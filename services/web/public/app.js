import "@picocss/pico";
import Chart from "chart.js/auto";
import "chartjs-adapter-moment";
import mqtt from "mqtt";

(async function () {
  const response = await fetch("/config/config.json");
  const config = await response.json();
  console.log("Loaded config:", config);
  let graphOptions = config.mqtt.data;

  const list = document.getElementById("phase-options");
  Object.entries(graphOptions).forEach(([key, value]) => {
    const li = document.createElement("li");
    li.innerHTML = `
      <label>
        <input type="radio" name="phase" value="${key}" />
        ${value.label}
      </label>
    `;
    list.appendChild(li);
  });

  const client = mqtt.connect(config.mqtt.ws);
  client.on("connect", () => {
    console.log("Connected to MQTT broker.");
    client.subscribe("can/A0");
    client.subscribe("can/766");
    client.subscribe("can/380");
    client.subscribe("can/AB");
    client.subscribe("logger/status");
  });

  let barChart;
  let lineChart;
  let selectedField = "";
  let selectedTopic = "";
  const MAX_POINTS = 1000;
  const logButton = document.getElementById('logCanData');

  const vcuState = document.getElementById("vcu-state");
  const peiState = document.getElementById("pei-state");
  const mcState = document.getElementById("mc-state");

  client.on("message", (topic, message) => {
      if (topic === "can/A0" && barChart) {
        const msg = JSON.parse(message.toString());
        const dataset = barChart.data.datasets[0].data;
        dataset[0] = msg.inv_module_a_temp;
        dataset[1] = msg.inv_module_b_temp;
        dataset[2] = msg.inv_module_c_temp;
        barChart.update();
      }

      if (topic === "can/766" && vcuState) {
        const msg = JSON.parse(message.toString());
        vcuState.textContent = msg.dashboard_state;
      }

      if (topic === "can/380" && peiState) {
        const msg = JSON.parse(message.toString());
        peiState.textContent = msg.pei_bms_status;
      }

      if (topic === "can/AB" && mcState) {
        const msg = JSON.parse(message.toString());
        if (msg.inv_run_fault !== "Normal")
          mcState.textContent = msg.inv_run_fault;
        else
          mcState.textContent = msg.inv_post_fault;
      }

      if (topic === "logger/status" && logButton) {
        const status = message.toString();
        if (status === "on") {
          logButton.value = "true";
          logButton.textContent = "Stop Log";
          logButton.className = "secondary";
        }
        else {
          logButton.value = "false";
          logButton.textContent = "Log CAN";
          logButton.className = "outline";
        }
      }

      if (topic === selectedTopic && lineChart) {
        const msg = JSON.parse(message.toString());
        const dataset = lineChart.data.datasets[0].data;
        dataset.push({ x: msg.timestamp, y: msg[selectedField] });

        if (dataset.length > MAX_POINTS) {
            dataset.splice(0, dataset.length - MAX_POINTS);
        }

        lineChart.update('none');
      }
  });

  const lineData = {
    datasets: [
      {
        label: "No Data Selected",
        data: [],
        backgroundColor: "rgb(135, 139, 219)",
      },
    ],
  };

  lineChart = new Chart(document.getElementById("lineChart"), {
    type: "line",
    data: lineData,
    options: {
      parsing: false,
      normalized: true,
      scales: {
        x: {
          ticks: {
            sampleSize: 10,
            maxTicksLimit: 10
          },
          type: "time",
          time: {
            unit: "second",
          },
          title: {
            display: true,
            text: "Time",
          },
        },
        y: {
          title: {
            display: true,
            text: "No Data Selected",
          },
        },
      }
    },
  });

  const labels = ["Module A", "Module B", "Module C"];
  const data = {
    labels: labels,
    datasets: [
      {
        label: "Inverter Module Temperatures",
        data: [0, 0, 0],
        backgroundColor: [
          "rgba(255, 99, 132, 0.2)",
          "rgba(255, 159, 64, 0.2)",
          "rgba(255, 205, 86, 0.2)",
        ],
        borderColor: [
          "rgb(255, 99, 132)",
          "rgb(255, 159, 64)",
          "rgb(255, 205, 86)",
        ],
        borderWidth: 1,
      },
    ],
  };

  barChart = new Chart(document.getElementById("barChart"), {
    type: "bar",
    data: data,
    options: {
      parsing: false,
      normalized: true,
      scales: {
        y: {
          beginAtZero: true,
          title: {
            display: true,
            text: "Temperature [C]",
          },
        },
      },
    },
  });

  const graphSelect = document.querySelectorAll('input[name="phase"]');
  graphSelect.forEach((radio) => {
    radio.addEventListener("change", (e) => {
      if (selectedField !== e.target.value) {
        client.unsubscribe(selectedTopic);
        selectedField = e.target.value;
        selectedTopic = graphOptions[selectedField].topic;
        client.subscribe(selectedTopic);

        lineChart.data.datasets[0].label = graphOptions[selectedField].label;
        lineChart.data.datasets[0].data.length = 0;
        lineChart.options.scales.y.title.text = graphOptions[selectedField].label;
        lineChart.update('none');
      }
    });
  });

  async function updateLogCanButton() {
    if (logButton.value == "false") {
      client.publish("logger/control", "on");
      console.log("Started logging CAN");
    } else {
      client.publish("logger/control", "off");
      console.log("Stopped logging CAN");
    }
  }
  document.getElementById('logCanData').addEventListener('click', updateLogCanButton);
})();