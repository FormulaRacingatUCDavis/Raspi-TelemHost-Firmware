<script lang="ts">
	import { onMount, onDestroy } from 'svelte';
	import Plyr from 'plyr';
	import 'plyr/dist/plyr.css';

	import LogDisplay from '$lib/components/LogDisplay.svelte';

	let selectedLog = $state('');

	let videoEl: HTMLVideoElement;
	let player: Plyr;

	onMount(() => {
		player = new Plyr(videoEl);
	});

	onDestroy(() => {
		player?.destroy();
	});
</script>

<LogDisplay source="camera" log={selectedLog} logPath="ready" />
<video
	bind:this={videoEl}
	controls
	playsinline
	style="width: 600px;"
></video>