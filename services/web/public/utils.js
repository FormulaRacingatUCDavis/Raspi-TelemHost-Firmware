const Dropdown = () => {
    const details = document.getElementById("logDropdown");
    const logOptions = document.getElementById("logOptions");
    const downloadButton = document.createElement("li");
    downloadButton.innerHTML = `<a href="#" id="fileDownload">Download Raw</a>`;

    downloadButton.querySelector("a").addEventListener("click", async (e) => {
        e.preventDefault();
        const selectedFiles = Array.from(
            logOptions.querySelectorAll('input[type="checkbox"]:checked')
        ).map(cb => cb.value);
        if (!selectedFiles.length) return alert("No files selected");

        const response = await fetch("/api/canlogs", {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({ filenames: selectedFiles })
        });

        const blob = await response.blob();
        const url = URL.createObjectURL(blob);
        const a = document.createElement("a");
        a.href = url;
        a.download = "logs.zip";
        document.body.appendChild(a);
        a.click();
        a.remove();
        URL.revokeObjectURL(url);
    });

    details.addEventListener("toggle", async () => {
        if (!details.open)
            return;

        const logs = await (await fetch("/api/canlog-list")).json();

        if (logs.length === 0) {
            logOptions.innerHTML = `<li>No files available</li>`;
        } else {
            logOptions.innerHTML = logs.map(log =>
                `<li>
                    <label>
                        <input type="checkbox" value="${log.file_name}">
                        ${log.file_name}
                    </label>
                </li>`
            ).join("");
            logOptions.appendChild(downloadButton);
        }
    });
};

const LogButton = (client) => {
    const logButton = document.getElementById("logCanData");

    client.on("message", (topic, message) => {
        if (topic !== "logger/status")
            return;
        const status = message.toString();
        if (status === "on") {
            logButton.value = "true";
            logButton.textContent = "Stop Log";
            logButton.className = "outline secondary";
        } else if (status === "off") {
            logButton.value = "false";
            logButton.textContent = "Start Log";
            logButton.className = "secondary";
        }
    });

    logButton.addEventListener("click", () => {
        if (logButton.value === "false") {
            client.publish("logger/control", "on");
        } else {
            client.publish("logger/control", "off");
        }
    });
};


export { Dropdown, LogButton };