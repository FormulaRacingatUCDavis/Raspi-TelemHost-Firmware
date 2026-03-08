<script lang="ts">
	import { onMount } from 'svelte';
	import { client as mqttStore } from '$lib/mqtt.svelte';
	import type { MqttClient } from 'mqtt';
	let { control, status, request } = $props();
	let mqttClient: MqttClient | null = null;

	onMount(() => {
		const unsubscribe = mqttStore.subscribe(c => {
			mqttClient = c;
		});
		return unsubscribe;
	});

	function handleClick() {
		if (!mqttClient) return;
		const payload = JSON.stringify({ status: status === 'off' ? 'on' : 'off' });
		mqttClient.publish(control, payload, { qos: 1 });
	}
</script>

<!-- svelte-ignore state_referenced_locally -->
<button class={status === "on" ? "outline contrast" : "contrast"} onclick={handleClick}>{request}</button>
