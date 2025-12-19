#include <WiFi.h>
#include <LocalClassifier.h>

// ==================== Settings ====================
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";
const char* serverUrl = "http://192.168.1.100:8000"; // Replace with your server IP

LocalClassifier classifier(serverUrl);

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // 1. Connect to WiFi
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // 2. Health Check
  Serial.println("\n--- Checking Server Health ---");
  if (classifier.healthCheck()) {
    Serial.println("✅ Server is online and healthy.");
  } else {
    Serial.println("❌ Server check failed: " + classifier.getLastError());
    Serial.println("Please check serverUrl and ensure server is running.");
    return; // Stop if server is not reachable
  }

  // ==================== Text Classification Demo ====================
  Serial.println("\n--- Text Classification Demo ---");
  
  // Classify
  String query = "hello world";
  Serial.println("Classifying: " + query);
  String result = classifier.classifyText(query);
  Serial.println("Result: " + result);

  // Parse Result
  String label = classifier.getJsonValue(result, "label");
  String confidence = classifier.getJsonValue(result, "confidence");
  Serial.println("Label: " + label);
  Serial.println("Confidence: " + confidence);


  // ==================== Image Classification Demo ====================
  Serial.println("\n--- Image Classification Demo (Base64) ---");
  
  // Dummy Base64 Image (Small white 1x1 pixel JPEG)
  String base64Image = "/9j/4AAQSkZJRgABAQEAYABgAAD/2wBDAAgGBgcGBQgHBwcJCQgKDBQNDAsLDBkSEw8UHRofHh0aHBwgJC4nICIsIxwcKDcpLDAxNDQ0Hyc5PTgyPC4zNDL/2wBDAQkJCQwLDBgNDRgyIRwhMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjL/wAARCAABAAEDASIAAhEBAxEB/8QAHwAAAQUBAQEBAQEAAAAAAAAAAAECAwQFBgcICQoL/8QAtRAAAgEDAwIEAwUFBAQAAAF9AQIDAAQRBRIhMUEGE1FhByJxFDKBkaEII0KxwRVS0fAkM2JyggkKFhcYGRolJicoKSo0NTY3ODk6Q0RFRkdISUpTVFVWV1hZWmNkZWZnaGlqc3R1dnd4eXqDhIWGh4iJipKTlJWWl5iZmqKjpKWmp6ipqrKztLW2t7i5usLDxMXGx8jJytLT1NXW19jZ2uHi4+Tl5ufo6erx8vP09fb3+Pn6/8QAHwEAAwEBAQEBAQEBAQAAAAAAAAECAwQFBgcICQoL/8QAtREAAgECBAQDBAcFBAQAAQJ3AAECAxEEBSExBhJBUQdhcRMiMoEIFEKRobHBCSMzUvAVYnLRChYkNOEl8RcYGRomJygpKjU2Nzg5OkNERUZHSElKU1RVVldYWVpjZGVmZ2hpanN0dXZ3eHl6goOEhYaHiImKkpOUlZaXmJmaoqOkpaanqKmqsrO0tba3uLm6wsPExcbHyMnK0tPU1dbX2Nna4uPk5ebn6Onq8vP09fb3+Pn6/9oADAMBAAIRAxEAPwD3+iiigD//2Q==";
  
  Serial.println("Classifying Base64 Image...");
  String imgResult = classifier.classifyImageBase64(base64Image);
  Serial.println("Image Result: " + imgResult);
  
  String imgLabel = classifier.getJsonValue(imgResult, "class");
  Serial.println("Image Class: " + imgLabel);

  Serial.println("\nDemo Complete.");
}

void loop() {
  // Do nothing
}
