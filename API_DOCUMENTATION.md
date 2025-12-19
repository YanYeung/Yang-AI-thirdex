# API Documentation

This document outlines the API endpoints for the Local Classifier Server, covering both Text Classification and Image Classification services.

## Base URL
`http://localhost:8000`

---

## 1. Text Classification API

### 1.1 Class Management

#### Get All Classes
Retrieves a list of all defined text classes and their associated samples.
- **Endpoint:** `GET /classes`
- **Response:**
  ```json
  {
    "classes": [
      {
        "name": "Class Name",
        "sample_count": 10,
        "samples": [
          {
            "id": "uuid",
            "text": "Sample text",
            "label": "Class Name"
          }
        ]
      }
    ]
  }
  ```

#### Add Class
Creates a new text classification category.
- **Endpoint:** `POST /classes`
- **Query Parameters:**
  - `class_name` (string): The name of the new class.
- **Response:**
  ```json
  { "message": "Class added" }
  ```

#### Rename Class
Renames an existing class and updates all associated samples.
- **Endpoint:** `PUT /classes/{class_name}`
- **Query Parameters:**
  - `new_name` (string): The new name for the class.
- **Response:**
  ```json
  { "message": "Class renamed" }
  ```

#### Delete Class
Deletes a class and all its samples.
- **Endpoint:** `DELETE /classes/{class_name}`
- **Response:**
  ```json
  { "message": "Class deleted" }
  ```

### 1.2 Sample Management

#### Add Sample
Adds a training text sample to a specific class.
- **Endpoint:** `POST /samples/{class_name}`
- **Body:**
  ```json
  {
    "id": "unique-id",
    "text": "The text content to train on",
    "label": "Class Name"
  }
  ```
- **Response:**
  ```json
  { "message": "Sample added" }
  ```

#### Delete Sample
Removes a specific sample from a class.
- **Endpoint:** `DELETE /samples/{class_name}/{sample_id}`
- **Response:**
  ```json
  { "message": "Sample deleted" }
  ```

### 1.3 Model Training & Prediction

#### Train Model
Triggers the training process for the text classifier.
- **Endpoint:** `POST /train`
- **Body:**
  ```json
  {
    "test_size": 0.2,
    "model_type": "tfjs"
  }
  ```
- **Response:**
  ```json
  {
    "message": "Training complete (Transfer Learning)",
    "accuracy": 0.95
  }
  ```

#### Classify Text (Public API)
Classifies a given text string.
- **Endpoint:** `POST /api/classify`
- **Body:**
  ```json
  {
    "text": "Text to classify"
  }
  ```
- **Response:**
  ```json
  {
    "category": "Predicted Class",
    "confidence": 0.98,
    "timestamp": "2024-01-01T12:00:00Z"
  }
  ```

#### Export Model
Exports the trained model in TensorFlow.js format.
- **Endpoint:** `GET /export`
- **Response:** `model.zip` file download.

---

## 2. Image Classification API

### 2.1 Class Management

#### Get All Classes
Retrieves all image classes and their sample images.
- **Endpoint:** `GET /api/image/classes`
- **Response:**
  ```json
  [
    {
      "name": "Class Name",
      "samples": [
        {
          "id": "image_filename.jpg",
          "url": "/images/Class Name/image_filename.jpg"
        }
      ]
    }
  ]
  ```

#### Add Class
Creates a new image classification category.
- **Endpoint:** `POST /api/image/classes`
- **Query Parameters:**
  - `class_name` (string): Name of the new class.
- **Response:**
  ```json
  { "message": "Class 'Name' added." }
  ```

#### Rename Class
Renames an existing image class.
- **Endpoint:** `PUT /api/image/classes/{class_name}`
- **Query Parameters:**
  - `new_name` (string): The new name for the class.
- **Response:**
  ```json
  { "message": "Class 'Old' renamed to 'New'." }
  ```

#### Delete Class
Deletes an image class and all its samples.
- **Endpoint:** `DELETE /api/image/classes/{class_name}`
- **Response:**
  ```json
  { "message": "Class 'Name' deleted." }
  ```

### 2.2 Sample Management

#### Add Sample Image
Uploads an image sample to a class.
- **Endpoint:** `POST /api/image/samples/{class_name}`
- **Body:** Multipart Form Data
  - `file`: (Binary image file)
- **Response:**
  ```json
  {
    "id": "generated_filename.jpg",
    "message": "Sample added"
  }
  ```

#### Delete Sample Image
Removes a specific image sample.
- **Endpoint:** `DELETE /api/image/samples/{class_name}/{sample_id}`
- **Response:**
  ```json
  { "message": "Sample deleted" }
  ```

### 2.3 Model Training & Prediction

#### Train Model
Starts the background training process for the image classifier.
- **Endpoint:** `POST /api/image/train`
- **Body:**
  ```json
  {
    "epochs": 10,
    "batch_size": 16,
    "preprocessing": {
      "interp": "bilinear",
      "padding": false
    }
  }
  ```
- **Response:**
  ```json
  { "message": "Training started" }
  ```

#### Get Training Status
Polls the current status of the training process.
- **Endpoint:** `GET /api/image/train/status`
- **Response:**
  ```json
  {
    "is_training": true,
    "epoch": 1,
    "total_epochs": 10,
    "batch": 5,
    "total_batches": 100,
    "loss": 0.5,
    "accuracy": 0.8,
    "status": "training",
    "estimated_remaining": 60.5
  }
  ```

#### Classify Image
Classifies an uploaded image.
- **Endpoint:** `POST /api/image/classify`
- **Body:** Multipart Form Data
  - `file`: (Binary image file)
- **Response:**
  ```json
  {
    "class": "Predicted Class",
    "confidence": {
      "Class A": 0.95,
      "Class B": 0.05
    },
    "timestamp": "2024-01-01T12:00:00Z"
  }
  ```

---

## 3. General

#### Health Check
Checks if the server and models are loaded and ready.
- **Endpoint:** `GET /health`
- **Response:**
  ```json
  {
    "status": "healthy",
    "timestamp": "...",
    "model_loaded": true,
    "embedding_model_loaded": true
  }
  ```
