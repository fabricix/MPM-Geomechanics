\page development_manual Development manual

\section development_manual_keywords Keywords for commit messages

This section provides a list of keywords to include in commits comments. These keywords help organize and identify different types of contributions for better readability and maintainability.

## Keywords List

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

\section development_manual_best_practices Best Practices

1. **Consistency**:
   - Always use these keywords in lowercase to maintain clarity.
2. **Combination**:
   - Combine keywords if necessary to clarify intent. For example:
     ```cpp
     // feat: Added error handling for invalid inputs
     // fix: Corrected edge case with empty arrays
     ```


\section development_manual_parallelization_guidelines Parallelization Guidelines

## Parallelization Guidelines in MPM-Geomechanics

This project uses OpenMP to accelerate selected functions through multi-threaded parallelization.

To maintain clarity and avoid unnecessary code duplication, we adopt the following convention:

---

### Use macro-controlled dual versions **only when needed**

Separate sequential and parallel versions (using `#if defined(...)`) **should be used only** when the function requires thread-local storage such as:

- **Mass or force accumulation** into nodal values
- **Boolean flags** (e.g. node activation)
- **Temporary vectors** shared across threads

**Examples:**  
- `nodalMass`  
- `nodalMomentum`  
- `nodalInternalForce`

---

### Use a single-version pattern with conditional pragma otherwise

If the function **does not require thread-local storage**, and the parallel and sequential logic is the same, then use this structure:

```cpp
#ifdef _OPENMP
    #pragma omp parallel for
#endif
for (int i = 0; i < N; ++i) {
    // loop body
}
