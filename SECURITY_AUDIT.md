# Security Audit Report: TuneBar Application

**Date:** October 26, 2023 (or current date)
**Target:** TuneBar ESP32-S3 Firmware
**Scope:** Source code analysis of the `sketch/` directory, focusing on network communications, data storage, and the OTA update mechanism.

## Executive Summary
This document presents a comprehensive security audit of the TuneBar ESP32-S3 application. The application incorporates network functionality (WiFi, HTTP/HTTPS requests), an OTA update mechanism, and local file storage (LittleFS, SD Card). While the application provides rich features, several critical and high-severity security vulnerabilities were identified that could allow attackers to perform Man-in-the-Middle (MITM) attacks, execute arbitrary code via malicious firmware updates, extract sensitive credentials, and abuse hardcoded API keys.

---

## 1. Critical Vulnerabilities

### 1.1 Insecure TLS/SSL Certificate Validation (CWE-295)
**Location:**
- `sketch/src/network/network.cpp` (in `fetchUrlData`)
- `sketch/src/updater/updater.cpp` (in `newFirmwareAvailable` and `ota_task`)

**Description:**
The application utilizes `WiFiClientSecure` to make HTTPS requests to fetch weather data, check for firmware updates (`tunebar_manifest.json`), and download the OTA firmware binary (`tunebar.bin`). However, it explicitly disables TLS certificate validation by calling `clientSecure.setInsecure()` and `otaClient.setInsecure()`.

**Impact:**
This completely defeats the purpose of HTTPS. An attacker on the same local network or in control of DNS/routing can easily perform a Man-in-the-Middle (MITM) attack. They could intercept the traffic and:
- Inject malicious firmware binaries during the OTA process, leading to a full device compromise.
- Spoof the firmware manifest to downgrade the device to a vulnerable version.
- Intercept or manipulate weather API requests.

**Recommendation:**
Remove `.setInsecure()`. Provide a trusted Root Certificate Authority (CA) bundle using `.setCACert()` to validate the server's certificate. For memory-constrained devices, consider pinning the public key or the certificate of the specific domains being accessed (`api.weatherapi.com` and `vaandcob.github.io`).

---

### 1.2 Lack of Cryptographic Firmware Authentication (CWE-345)
**Location:**
- `sketch/src/updater/updater.cpp`

**Description:**
The Over-The-Air (OTA) update process relies entirely on the transport layer (HTTPS) to ensure the integrity and authenticity of the downloaded firmware. It does not verify any cryptographic signature embedded in or associated with the firmware binary.

**Impact:**
Coupled with Vulnerability 1.1 (Insecure TLS), this allows an attacker to feed an arbitrary malicious binary to the device, which it will blindly install and execute. Even if TLS validation is fixed, if the GitHub repository or CDN is compromised, malicious firmware could still be distributed.

**Recommendation:**
Implement a cryptographic signature verification mechanism (e.g., Ed25519 or ECDSA). The device should hold a hardcoded public key, and the firmware binary (or an accompanying signature file) should be verified against this key before the `Update.end(true)` command is issued and the device reboots.

---

## 2. High Severity Vulnerabilities

### 2.1 Hardcoded API Credentials (CWE-798)
**Location:**
- `sketch/src/weather/weather.cpp` (Line 7: `const char *weatherApiKey = "1f00c88f3483483a9ba62101250612";`)

**Description:**
The API key for `weatherapi.com` is hardcoded directly into the source code.

**Impact:**
Anyone who downloads the firmware, extracts the binary, or views the open-source repository can easily extract this API key. Attackers can abuse this key in their own applications, potentially leading to rate limiting, denial of service for legitimate TuneBar users, or unexpected billing charges for the developer.

**Recommendation:**
Do not hardcode API keys. Instead:
- Require the user to provide their own API key via a configuration file (e.g., `config.json` on the SD card or LittleFS).
- Alternatively, proxy the requests through a developer-controlled backend server that holds the API key and enforces strict rate limits per IP/device.

---

### 2.2 Cleartext Storage of Sensitive Information (CWE-312)
**Location:**
- `sketch/src/network/network.cpp` (in `saveWifiList`)

**Description:**
The application stores user WiFi credentials (SSID and password) in plaintext within the `wifi.json` file on the LittleFS partition.

**Impact:**
If an attacker gains physical access to the device or extracts the flash memory, they can easily read the plaintext WiFi passwords. This could compromise the user's local network security.

**Recommendation:**
While full encryption of local storage on ESP32 without an external secure element can be challenging, utilizing the ESP32's NVS (Non-Volatile Storage) encryption feature is strongly recommended. NVS can securely store WiFi credentials instead of writing them to a plaintext JSON file.

---

## 3. Medium/Low Severity Findings & Best Practices

### 3.1 Cleartext Transmission for Internet Radio (CWE-319)
**Location:**
- `README.md` ("Only `http` URLs are supported; `https` is not allowed.")
- `sketch/src/file/file.cpp` (in `parseCSVLine` skipping `https://`)

**Description:**
The application explicitly disables support for HTTPS streams for Internet Radio, relying solely on unencrypted HTTP.

**Impact:**
While audio streams are generally not highly sensitive, transmitting them over HTTP allows anyone on the network to monitor what the user is listening to or inject malicious audio/noise into the stream.

**Recommendation:**
While likely a performance/memory constraint decision (ESP32 RAM limits when decoding audio + handling TLS), it is recommended to explore supporting HTTPS streams by optimizing TLS buffer sizes (`mbedtls` configurations) or offloading TLS if possible.

### 3.2 Robustness of JSON Parsing
**Location:**
- `sketch/src/network/network.cpp` (in `sanitizeJson`)

**Description:**
The custom `sanitizeJson` function manipulates memory strings using `memmove` based on `strlen` and `strrchr`. While it appears safe because `fetchUrlData` enforces null-termination (`outBuf[idx] = '\0';`), careful bounds checking must be maintained if the networking logic changes, to prevent buffer over-reads or overflows.

**Recommendation:**
Ensure that all incoming network payloads are strictly bounded and null-terminated before any string manipulation functions are invoked. Consider relying entirely on `ArduinoJson`'s built-in robust parsing capabilities if possible, rather than manually pre-sanitizing strings.

---

## Conclusion
The TuneBar application requires immediate attention regarding its network security posture. The combination of insecure TLS validation and the lack of cryptographic firmware signatures represents a critical risk of complete device compromise. Remediating these issues, along with securing the hardcoded API key and WiFi credentials, will significantly enhance the security and privacy of the device.