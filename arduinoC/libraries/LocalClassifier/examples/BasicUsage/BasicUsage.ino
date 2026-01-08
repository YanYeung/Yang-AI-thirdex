#include <WiFi.h>
#include <LocalClassifier.h>

// ==================== Settings ====================
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";
// Option 1: Full URL
// const char* serverUrl = "http://192.168.1.100:8000"; 
// LocalClassifier classifier(serverUrl);

// Option 2: Default constructor + setBaseUrl with IP and Port
LocalClassifier classifier;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // Set Server IP and Port
  classifier.setBaseUrl("192.168.1.100", 8000);
  
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
  Serial.println("\n--- Image Classification Demo (Camera Raw) ---");
  
  // Directly capture and classify image from camera
  // Ensure camera is initialized in your setup (e.g. k10.begin())
  // Here we assume camera is ready for demo purpose logic
  
  Serial.println("Capturing and Classifying Image...");
  String imgResult = classifier.classifyImageRaw(30); // Quality 30
  Serial.println("Image Result: " + imgResult);
  
  // Parse Result (Server should return JSON)
  String imgLabel = classifier.getJsonValue(imgResult, "class");
  Serial.println("Image Class: " + imgLabel);

  // ==================== Local OCR Demo ====================
  Serial.println("\n--- Local OCR Demo (Camera Raw) ---");
  Serial.println("Capturing and Reading Text...");
  // Rotate 0 degrees, quality 30
  String ocrText = classifier.localOcrRaw(30, 0); 
  Serial.println("OCR Result: " + ocrText);

  Serial.println("\nDemo Complete.");
}

void loop() {
  // Do nothing
}
