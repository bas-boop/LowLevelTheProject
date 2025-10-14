# Collision Crisis

### First run (no changes made)
* 12 FPS
* 2,500 balls

---

### Second run (changed vector into set)
* 5 FPS
* 2,500 balls
* **−54.5 %** compared to previous run

---

### Third run (changed set back to vector)
* 6 FPS
* 2,500 balls
* **+20 %** compared to previous run

---

### Fourth run (with spatial hashing, but collision broken)
* 82 FPS
* 2,500 balls
* **+1,266.7 %** compared to previous run

---

### Fifth run (with spatial hashing, forgot to call `buildSpatialGrid()`)
* 71 FPS
* 2,500 balls
* **−13.4 %** compared to previous run

---

### Sixth run (release mode instead of debug + code cleanup)
* 180 FPS (5,000 balls)  
  or
* 1.1 FPS (250,000 balls)

*(⚠️ different number of balls — no direct % comparison to previous run.)*

**FPS per ball:**
- 180 / 5,000 = 0.036
- 71 / 2,500 (previous) = 0.0284  
  **→ +26.8 % improvement (per ball)**

---

### Seventh run (major code cleanup and number optimization)
* 196.155 FPS (7,500 balls)  
  or
* 1.4 FPS (250,000 balls)
* **+27.3 %** compared to previous run at 250,000 balls

## 🏁 Conclusion

| Run                    | FPS   | Balls      | FPS per Ball       | Total Change vs First |
|-------------------------|-------|------------|---------------------|-------------------------|
| First run               | 12    | 2,500      | 0.0048              | —                       |
| Seventh run             | 196.155 | 7,500    | 0.02615             | **+1,534.6 %** (total FPS) |
| Seventh run (per ball)  | —     | —          | 0.02615 vs 0.0048   | **+445.8 %** (per ball) |

- From the **first** to the **seventh** run:
    - **Total FPS increased** from 12 to 196.155 for a larger number of balls (7,500 instead of 2,500).
    - That’s a **+1,534.6 %** improvement in total FPS.
    - When normalized per ball, performance improved by **+445.8 %**.
- The biggest single gain came from **spatial hashing**, and further cleanups/optimizations solidified the performance.
- The simulation also scaled up **from 2,500 balls to 250,000 balls**, maintaining measurable FPS.