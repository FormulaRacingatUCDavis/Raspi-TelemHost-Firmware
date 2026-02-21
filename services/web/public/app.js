import "@picocss/pico";
import Chart from "chart.js/auto";
import "chartjs-adapter-moment";
import mqtt from "mqtt";

/* Connect to MQTT broker from web browser using WebSockets */

const client = mqtt.connect('ws://192.168.137.100:8080');

client.on("connect", () => {
  console.log("Connected to MQTT broker.");
  client.subscribe("can/A0");
  client.subscribe("can/C0"); // torque request
  client.subscribe("can/A5"); // motor speed
  client.subscribe("can/388"); // current
  client.subscribe("can/381"); // state of charge
});

/* Graph live data */

let lineChart;
let barChart;

const MAX_POINTS = 1000;

// store graph data for different options
const storedData = {
  torque: [],
  speed: [],
  current: [],
  soc: [],
};

// for current graph select
let currentGraphSelect = "torque";

client.on("message", (topic, message) => {
  if (topic === "can/A0") {
    const msg = JSON.parse(message.toString());

    if (barChart) {
      const dataset = barChart.data.datasets[0].data;

      dataset[0] = msg.inv_module_a_temp;
      dataset[1] = msg.inv_module_b_temp;
      dataset[2] = msg.inv_module_c_temp;

      barChart.update();
    }
  } else if (topic === "can/C0") {
    //torque request
    const msg = JSON.parse(message.toString());

    storedData.torque.push({ x: msg.timestamp, y: msg.dashboard_torque });
    if (storedData.torque.length > MAX_POINTS) {
      storedData.torque.splice(0, storedData.torque.length - MAX_POINTS);
    }

    if (lineChart && currentGraphSelect === "torque") {
      lineChart.data.datasets[0].data = storedData.torque;
      lineChart.update("none");
    }
  } else if (topic === "can/A5") {
    // motor speed
    const msg = JSON.parse(message.toString());

    storedData.speed.push({ x: msg.timestamp, y: msg.inv_motor_speed });
    if (storedData.speed.length > MAX_POINTS) {
      storedData.speed.splice(0, storedData.speed.length - MAX_POINTS);
    }

    if (lineChart && currentGraphSelect === "speed") {
      lineChart.data.datasets[0].data = storedData.speed;
      lineChart.update("none");
    }
  } else if (topic === "can/388") {
    // current
    const msg = JSON.parse(message.toString());

    storedData.current.push({ x: msg.timestamp, y: msg.pei_current });
    if (storedData.current.length > MAX_POINTS) {
      storedData.current.splice(0, storedData.current.length - MAX_POINTS);
    }

    if (lineChart && currentGraphSelect === "current") {
      lineChart.data.datasets[0].data = storedData.current;
      lineChart.update("none");
    }
  } else if (topic === "can/381") {
    // state of charge
    const msg = JSON.parse(message.toString());

    storedData.soc.push({ x: msg.timestamp, y: msg.pei_soc });
    if (storedData.soc.length > MAX_POINTS) {
      storedData.soc.splice(0, storedData.soc.length - MAX_POINTS);
    }

    if (lineChart && currentGraphSelect === "soc") {
      lineChart.data.datasets[0].data = storedData.soc;
      lineChart.update("none");
    }
  }
});

(async function () {
  const lineData = {
    datasets: [
      {
        label: "Torque Request",
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
          beginAtZero: true,
          title: {
            display: true,
            text: "No Data Selected",
          },
        },
      },
      decimation: {
        enabled: true,
        algorithm: "min-max",
        samples: 1000,
      },
    },
  });

  const labels = ["Module A", "Module B", "Module C"];
  const data = {
    labels: labels,
    datasets: [
      {
        label: "Inverter Module Temperatures",
        data: [null, null, null],
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

  // dropdown menu options select
  const radioButtonSelect = document.querySelectorAll('input[name="phase"]');
  radioButtonSelect.forEach((radio) => {
    radio.addEventListener("change", (e) => {
      currentGraphSelect = e.target.value;

      switch (currentGraphSelect) {
        case "torque":
          lineChart.data.datasets[0].label = "Torque Request";
          lineChart.data.datasets[0].data = storedData.torque;
          lineChart.options.scales.y.title.text = "Nm";
          break;
        case "speed":
          lineChart.data.datasets[0].label = "Motor Speed";
          lineChart.data.datasets[0].data = storedData.speed;
          lineChart.options.scales.y.title.text = "RPM";
          break;
        case "current":
          lineChart.data.datasets[0].label = "Current";
          lineChart.data.datasets[0].data = storedData.current;
          lineChart.options.scales.y.title.text = "Amps";
          break;
        case "soc":
          lineChart.data.datasets[0].label = "State of Charge";
          lineChart.data.datasets[0].data = storedData.soc;
          lineChart.options.scales.y.title.text = "%";
          break;
      }

      lineChart.update();
    });
  });
})();
