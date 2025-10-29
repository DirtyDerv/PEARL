# PEARL Menu System Testing Guide

## 🧪 **Menu Testing Overview**

The PEARL firmware includes comprehensive menu testing capabilities through both serial commands and a web-based testing interface.

## 📡 **Serial Command Testing**

### **Standard Commands**
```
R - Reset position to zero
S - Scan I2C bus  
P - Toggle PIO/GPIO mode
V - Show velocity & performance info
D - Toggle display mode
C - Clear performance counters
M - Show menu encoder status
I - Show version info
H - Show this help
```

### **Menu Test Commands** ⭐
```
T - Test menu system status
U - Force user menu activation
E - Force engineering menu activation (bypass password)
Q - Force menu exit
```

## 🌐 **Web-Based Testing**

### **Setup**
1. Open `Web Test/index.html` in a modern web browser
2. Click "Connect to Pico" 
3. Select the PEARL device's serial port
4. Web interface will show menu test buttons when connected

### **Available Web Tests**
- **Run All Tests (Web Only)**: Validates web interface functionality
- **Run All Tests on Pico**: Comprehensive device testing sequence
- **Individual Menu Commands**: Direct menu control buttons

## 🔍 **Test Scenarios**

### **1. User Menu Test**
```
Command Sequence:
1. Send 'U' → Should activate user menu
2. Send 'T' → Should show menu active status  
3. Send 'Q' → Should exit menu
4. Send 'T' → Should show menu inactive status
```

**Expected Results:**
- LCD displays user menu options
- Serial shows "Activating user menu via serial command..."
- Menu navigation should work with HW-040 encoder

### **2. Engineering Menu Test**
```
Command Sequence:
1. Send 'E' → Should activate engineering menu (bypass password)
2. Send 'T' → Should show menu active status
3. Send 'M' → Should show menu encoder status
4. Send 'Q' → Should exit to main display
```

**Expected Results:**
- LCD displays engineering menu
- Serial confirms "Activating engineering menu... (bypassing password)"
- Full menu hierarchy should be accessible

### **3. Menu State Machine Test**
```
Command Sequence:
1. Send 'T' → Verify initial INACTIVE state
2. Send 'U' → Activate user menu
3. Send 'E' → Should fail (menu already active)  
4. Send 'Q' → Exit menu
5. Send 'E' → Should succeed (engineering menu)
6. Send 'U' → Should fail (menu already active)
7. Send 'Q' → Exit menu
```

**Expected Results:**
- State transitions follow proper logic
- Prevents multiple menu activations
- Clean state management

### **4. Hardware Integration Test**
```
Physical Test:
1. Triple-click HW-040 encoder → Should trigger password entry
2. Long-press HW-040 encoder → Should activate user menu
3. Rotate encoder → Should navigate menu items
4. Click encoder → Should select menu items
```

## 📊 **Test Results Validation**

### **Menu System Status Indicators**
- **Menu State**: ACTIVE/INACTIVE
- **Menu Encoder Position**: Current rotary position
- **Menu Encoder Status**: Hardware status string
- **Click Detection**: Triple-click and long-press events

### **Expected LCD Behavior**
```
Main Display → "Pos: XXX.XX" (when menu inactive)
User Menu → "USER MENU" with navigation options
Engineering Menu → "ENGINEERING MENU" with full options
Password Entry → "Enter Password: ****"
```

### **Expected Serial Output**
```
=== MENU SYSTEM TEST ===
Testing menu state machine...
Current menu state: INACTIVE
Menu encoder position: 0
Menu encoder status: OK
Test complete.
```

## 🐛 **Troubleshooting**

### **Common Issues**
1. **Menu Not Activating**: Check HW-040 encoder wiring (pins 12,13,14)
2. **Serial Commands Not Working**: Verify baud rate (115200)
3. **Web Interface Not Connecting**: Ensure browser supports Web Serial API
4. **Menu Display Issues**: Check I2C LCD wiring (pins 8,9)

### **Debug Commands**
```
M - Shows detailed menu encoder status
S - Scans I2C bus for LCD
V - Shows system performance metrics
I - Shows version and build information
```

## ✅ **Test Completion Checklist**

- [ ] Serial commands respond correctly
- [ ] User menu activates and navigates
- [ ] Engineering menu bypasses password in test mode
- [ ] Menu exits cleanly to main display
- [ ] HW-040 encoder responds to rotation/clicks
- [ ] LCD displays menu content correctly
- [ ] Web interface connects and controls device
- [ ] State machine prevents invalid transitions
- [ ] Performance monitoring shows no errors
- [ ] I2C bus scan detects LCD

## 🚀 **Advanced Testing**

### **Stress Test Sequence**
```javascript
// Web interface stress test
for (let i = 0; i < 10; i++) {
    await sendMenuCommand('U');  // Activate user menu
    await delay(500);
    await sendMenuCommand('Q');  // Exit menu  
    await delay(500);
    await sendMenuCommand('E');  // Activate engineering menu
    await delay(500);
    await sendMenuCommand('Q');  // Exit menu
    await delay(500);
}
```

### **Performance Validation**
- Monitor FIFO overflow counts during rapid menu switching
- Verify LCD refresh rates remain consistent
- Check memory usage during extended menu sessions
- Validate encoder responsiveness under load

## 📝 **Test Report Template**

```
PEARL Menu System Test Report
Date: ___________
Firmware Version: ___________
Hardware Configuration: ___________

[ ] Basic serial commands functional
[ ] Menu activation via serial works
[ ] Menu navigation via HW-040 works  
[ ] LCD display updates correctly
[ ] State machine logic correct
[ ] Web interface connectivity
[ ] Performance metrics acceptable
[ ] No memory leaks detected

Issues Found:
_________________________________

Recommendations:
_________________________________
```

The menu system is now fully testable both via serial commands and web interface!