const exportSelect = document.getElementById("logs");

async function list_logs()
{
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

window.addEventListener("DOMContentLoaded", list_logs);