\page development_manual Development manual

# Keywords for commit messages

This section provides a list of keywords to include in commits comments. These keywords help organize and identify different types of contributions for better readability and maintainability.

## **Keywords**

### **1. `doc:`**
- **Purpose**: For documentation-related comments or detailed explanations.
- **Example**:
  ```cpp
  // doc: General algorithm description for terrain contact
  ```

### **2. `feat:`**
- **Purpose**: To mark features or specific functionalities.
- **Example**:
  ```cpp
  // feat: Added support for binary STL mesh files
  ```

### **3. `fix:`**
- **Purpose**: To identify bug fixes or adjustments.
- **Example**:
  ```cpp
  // fix: Corrected input parameter validation
  ```

### **4. `refactor:`**
- **Purpose**: For restructuring code without changing its functionality.
- **Example**:
  ```cpp
  // refactor: Separated normal calculation module
  ```

### **5. `test:`**
- **Purpose**: For comments related to tests or test cases.
- **Example**:
  ```cpp
  // test: Validated force calculations in impact problem
  ```

### **6. `perf:`**
- **Purpose**: To highlight performance improvements or optimizations.
- **Example**:
  ```cpp
  // perf: Reduced memory usage in explicit solver
  ```

### **7. `todo:`**
- **Purpose**: To indicate pending tasks or features.
- **Example**:
  ```cpp
  // todo: Implement error handling for input keywords
  ```

### **8. `hack:`**
- **Purpose**: To identify temporary or workaround solutions.
- **Example**:
  ```cpp
  // hack: Using fixed values to bypass null mass in the mesh nodes
  ```

### **9. `warn:`**
- **Purpose**: For warnings about potential issues or limitations.
- **Example**:
  ```cpp
  // warn: Current update particle density method does not support particle shear changes
  ```

### **10. `debug:`**
- **Purpose**: For debugging or test-specific comments.
- **Example**:
  ```cpp
  // debug: Prints the result of calculated normals
  ```

### **11. `chore:`**
- **Purpose**: For minor maintenance or cleaning tasks.
- **Example**:
  ```cpp
  // chore: Removed unused code in mesh reading module
  ```

### **12. `deprecated:`**
- **Purpose**: To mark features or methods that are no longer in use.
- **Example**:
  ```cpp
  // deprecated: This method will be removed in the next version
  ```

## **Best Practices**

1. **Consistency**:
   - Always use these keywords in lowercase to maintain clarity.
2. **Combination**:
   - Combine keywords if necessary to clarify intent. For example:
     ```cpp
     // feat: Added error handling for invalid inputs
     // fix: Corrected edge case with empty arrays
     ```