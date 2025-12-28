#include "LocalClassifier.h"

LocalClassifier::LocalClassifier(String baseUrl) {
    if (baseUrl.endsWith("/")) {
        _baseUrl = baseUrl.substring(0, baseUrl.length() - 1);
    } else {
        _baseUrl = baseUrl;
    }
}

LocalClassifier::~LocalClassifier() {}

void LocalClassifier::setBaseUrl(String baseUrl) {
    if (baseUrl.endsWith("/")) {
        _baseUrl = baseUrl.substring(0, baseUrl.length() - 1);
    } else {
        _baseUrl = baseUrl;
    }
}

String LocalClassifier::getLastError() {
    return _lastError;
}

// ==================== Private Helpers ====================

String LocalClassifier::sendJsonRequest(String method, String endpoint, String payload) {
    if (WiFi.status() != WL_CONNECTED) {
        _lastError = "WiFi not connected";
        return "";
    }

    HTTPClient http;
    WiFiClient client;
    
    String url = _baseUrl + endpoint;
    
    if (!http.begin(client, url)) {
        _lastError = "Connection failed";
        return "";
    }

    http.addHeader("Content-Type", "application/json");
    // Increase timeout to 10 seconds for image upload
    http.setTimeout(10000); 

    int httpCode = 0;
    if (method == "GET") {
        httpCode = http.GET();
    } else if (method == "POST") {
        httpCode = http.POST(payload);
    } else if (method == "PUT") {
        httpCode = http.PUT(payload);
    } else if (method == "DELETE") {
        httpCode = http.sendRequest("DELETE", payload);
    } else {
        _lastError = "Invalid method";
        http.end();
        return "";
    }

    String response = "";
    if (httpCode > 0) {
        // Only read response if content length is reasonable or we expect a response
        // Note: getString() can fail allocation if response is too large
        response = http.getString();
        
        if (httpCode >= 200 && httpCode < 300) {
            _lastError = ""; 
        } else {
            _lastError = "HTTP " + String(httpCode) + ": " + response;
            if (httpCode >= 400) response = ""; 
        }
    } else {
        _lastError = "HTTP Error: " + http.errorToString(httpCode);
    }

    http.end();
    return response;
}

// ==================== 1. Text Classification API ====================

String LocalClassifier::classifyText(String text) {
    // Endpoint: POST /api/classify
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["text"] = text;

    String payload;
    root.printTo(payload);

    String response = sendJsonRequest("POST", "/api/classify", payload);
    
    // Parse the result to return only the label
    if (response.length() > 0) {
        String label = getJsonValue(response, "category");
        if (label.length() > 0) {
            return label;
        }
    }
    
    return response;
}

#include "OptimizedCameraUpload.h"

// ==================== 2. Image Classification API ====================

String LocalClassifier::classifyImageRaw(int quality) {
    // Construct the full URL for the raw image classification endpoint
    // Endpoint: POST /api/image/classify/raw
    String url = _baseUrl + "/api/image/classify/raw";
    
    // Reuse the optimized upload logic directly
    // This handles: queue fetch -> JPEG compression -> binary POST -> cleanup
    String response = uploadCameraFrameOptimized(url, quality);
    
    // Check if the response indicates a local error from the upload function
    if (response.startsWith("Error:")) {
        _lastError = response;
        return ""; // Return empty string on transport/hardware error
    }
    
    // If we got a response from the server, it should be JSON
    _lastError = "";
    if (response.length() > 0) {
        // Parse the result to return only the label
        String label = getJsonValue(response, "category");
        if (label.length() > 0) {
            return label;
        }
    }
    return response;
}

// ==================== 3. Baidu AI Proxy API (Raw Binary) ====================

String LocalClassifier::baiduOcrRaw(int quality) {
    // Endpoint: POST /api/baidu/ocr/raw
    String url = _baseUrl + "/api/baidu/ocr/raw";
    return uploadCameraFrameOptimized(url, quality);
}

String LocalClassifier::baiduFaceSearchRaw(int quality) {
    // Endpoint: POST /api/baidu/face/search/raw
    String url = _baseUrl + "/api/baidu/face/search/raw";
    return uploadCameraFrameOptimized(url, quality);
}

String LocalClassifier::baiduFaceDetectRaw(int quality) {
    // Endpoint: POST /api/baidu/face/detect/raw
    String url = _baseUrl + "/api/baidu/face/detect/raw";
    String response = uploadCameraFrameOptimized(url, quality);
    
    // 解析结果，仅返回类别标签
    if (response.length() > 0) {
        String label = getJsonValue(response, "class");
        if (label.length() > 0) {
            return label;
        }
    }
    
    return response;
}
// ==================== 3. General ====================

bool LocalClassifier::healthCheck() {
    String res = sendJsonRequest("GET", "/health");
    if (res.length() > 0 && _lastError == "") {
        return true;
    }
    return false;
}

String LocalClassifier::getJsonValue(String json, String key) {
    if (json.length() == 0) return "";
    
    // Use string manipulation instead of ArduinoJson to save memory and avoid parsing errors
    // Look for "key":"value" or "key" : "value"
    String keyPattern = "\"" + key + "\"";
    int keyIdx = json.indexOf(keyPattern);
    if (keyIdx == -1) return "";

    // Find the colon after the key
    int colonIdx = json.indexOf(':', keyIdx + keyPattern.length());
    if (colonIdx == -1) return "";

    // Find the start of the value (assuming it's a string starting with ")
    int valStart = json.indexOf('"', colonIdx + 1);
    if (valStart == -1) return "";
    
    // Find the end of the value
    // Note: This simple parser doesn't handle escaped quotes within the string, 
    // but for simple class names it should be sufficient.
    int valEnd = json.indexOf('"', valStart + 1);
    if (valEnd == -1) return "";
    
    return json.substring(valStart + 1, valEnd);
}
