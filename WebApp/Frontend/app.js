let BUS = "pcan";
let CAN_ID = 0xa0;
let SIGNAL = "INV_Module_A_Temp";

function makeWsUrl() {
    const WS_BASE = window.location.origin.replace(/^http/, "ws");
    return `${WS_BASE}/ws/${BUS}/${CAN_ID}/${SIGNAL}`;
}

const ctx = document.getElementById("rateChart").getContext("2d");

const rateChart = new Chart(ctx, {
    type: "line",
    data: {
        labels: [],
        datasets: [{
            label: `${SIGNAL} (${BUS} / ${CAN_ID})`,
            data: [],
            borderWidth: 2
        }]
    },
    options: {
        responsive: true,
        animation: false,
        scales: {
            x: { display: true },
            y: { beginAtZero: true }
        }
    }
});

let ws = null;

function connectWebSocket() {
    if (ws) ws.close();
    const url = makeWsUrl();
    ws = new WebSocket(url);
    ws.onopen = () => {};
    ws.onmessage = (event) => {
        const msg = JSON.parse(event.data);
        addPoint(msg.timestamp, msg.value);
    };
    ws.onerror = () => {};
    ws.onclose = () => {
        setTimeout(connectWebSocket, 1000);
    };
}

connectWebSocket();

function addPoint(ts, value) {
    rateChart.data.labels.push(ts.toFixed(2));
    rateChart.data.datasets[0].data.push(value);
    rateChart.update();
    if (rateChart.data.labels.length > 100) {
        rateChart.data.labels.shift();
        rateChart.data.datasets[0].data.shift();
    }
}

function changeStream() {
    const idBox = document.getElementById("canIdInput");
    const sigBox = document.getElementById("signalInput");
    let newId = idBox.value.trim();
    let newSig = sigBox.value.trim();
    if (!newId || !newSig) return;

    if (newId.startsWith("0x") || newId.startsWith("0X")) {
        CAN_ID = parseInt(newId, 16);
    } else {
        CAN_ID = parseInt(newId, 10);
    }

    SIGNAL = newSig;

    rateChart.data.labels = [];
    rateChart.data.datasets[0].data = [];
    rateChart.data.datasets[0].label = `${SIGNAL} (${BUS} / ${CAN_ID})`;
    rateChart.update();

    connectWebSocket();
}

async function startLogging() {
    rateChart.data.labels = [];
    rateChart.data.datasets[0].data = [];
    rateChart.update();
    const now = new Date().toISOString();
    const tz = Intl.DateTimeFormat().resolvedOptions().timeZone;
    await fetch(`/sync-time?now=${encodeURIComponent(now)}&tz=${encodeURIComponent(tz)}`, { method: "POST" });
    await fetch("/start", { method: "POST" });
}

async function stopLogging() {
    await fetch("/stop", { method: "POST" });
    window.location.href = "/download/pcan";
    setTimeout(() => {
        window.location.href = "/download/tcan";
    }, 300);
}