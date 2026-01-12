const exportSelect = document.getElementById("logs");
const logButton = document.getElementById("logData");

async function list_logs() {
    const response = await fetch("/list_logs");
    const data = await response.json();

    exportSelect.innerHTML = "";

    const files = data.files || [];
    files.forEach(file => {
        const option = document.createElement("option");
        option.value = file;
        option.textContent = file;
        exportSelect.appendChild(option);
    });
}

async function pollLogStatus() {
    try {
        const response = await fetch("/log_status");
        const data = await response.json();
        logButton.textContent = data.logging ? "Stop Recording" : "Start Recording";
    } catch (e) {
        console.error("Failed to fetch log status:", e);
    }

    setTimeout(pollLogStatus, 1000); // keep polling every 1 second
}

logButton.addEventListener("click", async () => {
    await fetch("/toggle_log", { method: "POST" });
    // status will be updated by pollLogStatus on next tick
});

window.addEventListener("DOMContentLoaded", () => {
    list_logs();
    pollLogStatus(); // start continuous polling immediately
});