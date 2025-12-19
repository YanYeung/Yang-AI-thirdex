#ifndef LOCAL_CLASSIFIER_H
#define LOCAL_CLASSIFIER_H

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

/**
 * @brief Client library for Local Classifier Server (Text & Image)
 * 
 * Implements simplified API endpoints for text and image classification.
 */
class LocalClassifier {
public:
    /**
     * @brief Constructor
     * @param baseUrl Base URL of the server (e.g., "http://192.168.1.100:8000")
     */
    LocalClassifier(String baseUrl = "http://localhost:8000");
    ~LocalClassifier();

    /**
     * @brief Set the Base URL
     * @param baseUrl New base URL
     */
    void setBaseUrl(String baseUrl);

    /**
     * @brief Get the last error message
     * @return Error string
     */
    String getLastError();

    // ==================== 1. Text Classification API ====================
    
    /**
     * @brief Classify text
     * @param text Text to classify
     * @return JSON String with classification result, or empty on failure
     */
    String classifyText(String text);

    // ==================== 2. Image Classification API ====================

    /**
     * @brief Classify an image from Base64 string
     * @param base64Image Base64 encoded image string
     * @return JSON String with classification result
     */
    String classifyImageBase64(String base64Image);

    // ==================== 3. General ====================

    /**
     * @brief Check server health
     * @return true if healthy
     */
    bool healthCheck();

    /**
     * @brief Extract value from JSON string
     * @param json JSON string
     * @param key Key to extract
     * @return Value as string
     */
    String getJsonValue(String json, String key);

private:
    String _baseUrl;
    String _lastError;

    // Helper to send JSON requests (GET, POST, PUT, DELETE)
    String sendJsonRequest(String method, String endpoint, String payload = "");
};

#endif
