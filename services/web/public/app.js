import "@picocss/pico";
import mqtt from "mqtt";
import { Dropdown, LogButton } from "./utils.js";
import Camera from "./camera.js";
import Graphs from "./graphs.js";

let config;
fetch("/resources/config.json")
    .then(res => res.json())
    .then(cfg => {
        config = cfg;
        const client = mqtt.connect(config.mqtt.ws);

        client.on("connect", () => {
            console.log("Connected to MQTT broker:", config.mqtt.ws);
            client.subscribe("logger/status");
            client.subscribe("can/A0");
            client.subscribe("can/766");
            client.subscribe("can/380");
            client.subscribe("can/AB");
        });

        Dropdown();
        LogButton(client);

        document.addEventListener("click", (e) => {
            const link = e.target.closest(".nav-link");
            if (!link) return;
            e.preventDefault();
            window.history.pushState({}, "", link.href);
            handleLocation();
        });

        const routes = {
            404: () => { document.getElementById("main-content").innerHTML = "<h2>404</h2>"; },
            "/": () => Graphs(client, config),
            "/camera": () => Camera(client)
        };

        function handleLocation() {
            const path = window.location.pathname;
            const route = routes[path] || routes["404"];
            route();
        }

        window.onpopstate = handleLocation;
        handleLocation();
    })
    .catch(err => console.error("Failed to load config:", err));