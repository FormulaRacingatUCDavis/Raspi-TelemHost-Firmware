<script lang="ts">
    import { onMount } from 'svelte';
    import { client as mqttStore } from '$lib/mqtt.svelte';
    import type { MqttClient } from 'mqtt';

    let { control, status, request } = $props();
    let mqttClient: MqttClient | null = $state(null);

    onMount(() => {
        const unsubscribe = mqttStore.subscribe(c => {
            mqttClient = c;
        });
        return unsubscribe;
    });

    function handleClick() {
        if (!mqttClient) return;
        const newStatus = status === 'off' ? 'on' : 'off';
        const payload = JSON.stringify({ status: newStatus });
        
        mqttClient.publish(control, payload, { qos: 1 });
    }
</script>

<button 
    type="button"
    class={status === "on" ? "outline contrast" : "contrast"} 
    onclick={handleClick}
>
    {request}
</button>

<style>
    button {
        width: 100%;
        margin-bottom: var(--pico-spacing);
    }
</style>