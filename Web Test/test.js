
// Web Serial API integration for Pico
let picoPort = null;
let picoReader = null;
let picoWriter = null;
let serialOutputDiv = null;

async function connectToPico() {
    try {
        picoPort = await navigator.serial.requestPort();
        await picoPort.open({ baudRate: 115200 });
        picoWriter = picoPort.writable.getWriter();
        picoReader = picoPort.readable.getReader();
        document.getElementById('run-pico-tests').disabled = false;
        serialOutputDiv = document.getElementById('serial-output');
        serialOutputDiv.innerText = '[Connected to Pico]\n';
        listenToPico();
    } catch (e) {
        alert('Failed to connect: ' + e);
    }
}

async function listenToPico() {
    let decoder = new TextDecoder();
    while (picoPort && picoReader) {
        try {
            const { value, done } = await picoReader.read();
            if (done) break;
            if (value) {
                serialOutputDiv.innerText += decoder.decode(value);
                serialOutputDiv.scrollTop = serialOutputDiv.scrollHeight;
            }
        } catch (e) {
            break;
        }
    }
}

async function runPicoTests() {
    if (!picoWriter) return;
    serialOutputDiv.innerText += '\n[Web] Sending ALL to Pico...\n';
    const encoder = new TextEncoder();
    await picoWriter.write(encoder.encode('ALL\n'));
}

document.addEventListener('DOMContentLoaded', () => {
    document.getElementById('connect-btn').onclick = connectToPico;
    document.getElementById('run-pico-tests').onclick = runPicoTests;
});

// Example test structure for PEARL project (web only)
function runAllTests() {
    const resultsDiv = document.getElementById('test-results');
    resultsDiv.innerHTML = '';
    let tests = [
        { name: 'Test Example 1', test: () => true },
        { name: 'Test Example 2', test: () => false },
        // Add more tests here as you implement them
    ];
    let allPassed = true;
    tests.forEach(t => {
        let passed = false;
        try {
            passed = t.test();
        } catch (e) {
            passed = false;
        }
        const result = document.createElement('div');
        result.className = passed ? 'test-pass' : 'test-fail';
        result.textContent = `${t.name}: ${passed ? 'PASS' : 'FAIL'}`;
        resultsDiv.appendChild(result);
        if (!passed) allPassed = false;
    });
    if (allPassed) {
        resultsDiv.innerHTML += '<div class="test-pass"><strong>All tests passed!</strong></div>';
    }
}
