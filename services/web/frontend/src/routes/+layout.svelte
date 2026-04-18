<script lang="ts">
	import '@picocss/pico';
	let { children } = $props();
	import LogButton from '$lib/components/LogButton.svelte';
	import { mqttDataStr } from '$lib/mqtt.svelte';
</script>

<div class="layout">
	<aside class="sidebar">
		<nav>
			<ul>
				<li>
					<a href="/"><img src="/logo.svg" alt="FRUCD Logo" style="height:70px; width:auto;" /></a>
				</li>
				<hr />
				<li>
					<LogButton
						control="can/log/control"
						status={$mqttDataStr.logStatus}
						request={$mqttDataStr.logReq}
					/>
				</li>
				<li>
					<input type="button" value="Record Camera" />
				</li>
				<hr />
				<li>
					<a
						href="/live/readings"
						role="button"
						class="outline contrast"
						style="display: block; width: 100%; text-align: center;"
					>
						Live
					</a>
				</li>
				<li>
					<a
						href="/logs/readings"
						role="button"
						class="outline contrast"
						style="display: block; width: 100%; text-align: center;"
					>
						Logs
					</a>
				</li>
				<li>
					<a
						href="/control"
						role="button"
						class="outline contrast"
						style="display: block; width: 100%; text-align: center;"
					>
						Control
					</a>
				</li>
			</ul>
		</nav>
	</aside>

	<main id="main-content" class="container">
		{@render children()}
	</main>
</div>

<!-- <script lang="ts">
	import '@picocss/pico';
	import LogButton from '$lib/components/LogButton.svelte';
	let { children } = $props();
	import { mqttDataStr } from '$lib/mqtt.svelte';

	type Log = { file_name: string };
	let files = $state<Log[]>([]);
	let selectedFiles = $state([]);

	async function updateDropdown() {
		const res = await fetch('/api/can/logs/list/raw');
		const data: Log[] = await res.json();
		files = data;
	}

	async function download(type: 'raw' | 'parsed') {
		const response = await fetch(`/api/can/logs/zip/${type}`, {
			method: 'POST',
			headers: { 'Content-Type': 'application/json' },
			body: JSON.stringify({ filenames: selectedFiles })
		});

		const blob = await response.blob();
		const url = URL.createObjectURL(blob);
		const a = document.createElement('a');
		a.href = url;
		a.download = 'canlogs.zip';
		document.body.appendChild(a);
		a.click();
		a.remove();
		URL.revokeObjectURL(url);

		console.log(selectedFiles);
	}
</script>

<header class="container">
	<nav>
		<ul>
			<li>
				<img src="/logo.svg" alt="FRUCD Logo" style="height:70px; width:auto; padding-top:10px" />
			</li>
		</ul>
		<ul>
			<li><a href="/" class="secondary nav-link">Home</a></li>
			<li><a href="/live/readings" class="secondary nav-link">Live</a></li>
			<li><a href="/logs/readings" class="secondary nav-link">Logs</a></li>
			<li><a href="/send" class="secondary nav-link">Send</a></li>
			<li>
				<details class="dropdown" ontoggle={updateDropdown}>
					<summary> Export </summary>
					<ul dir="rtl">
						{#if files.length > 0}
							{#each files as file}
								<li>
									<label dir="ltr">
										<input
											type="checkbox"
											name="phase"
											value={file.file_name}
											bind:group={selectedFiles}
										/>
										{file.file_name}
									</label>
								</li>
							{/each}
							<li>
								<input
									class="secondary"
									type="button"
									onclick={() => download('raw')}
									value="Raw"
								/>
							</li>
							<li>
								<input
									class="secondary"
									type="button"
									onclick={() => download('parsed')}
									value="Parsed"
								/>
							</li>
						{:else}
							<li>No logs available</li>
						{/if}
					</ul>
				</details>
			</li>
			<li>
				<LogButton
					control="can/log/control"
					status={$mqttDataStr.logStatus}
					request={$mqttDataStr.logReq}
				/>
			</li>
			<li>
				<button class="contrast" disabled>Record Camera</button>
			</li>
		</ul>
	</nav>
	<hr />
</header>

<main id="main-content" class="container">
	{@render children()}
</main> -->

<style>
	/* Desktop Layout */
	.layout {
		display: flex;
		gap: var(--pico-spacing);
		min-height: 100vh;
	}

	.sidebar {
		width: 250px;
		border-right: var(--pico-border-width) solid var(--pico-muted-border-color);
		padding: var(--pico-spacing);
		position: sticky;
		top: 0;
		height: 100vh;
		flex-shrink: 0;
	}

	.sidebar nav,
	.sidebar ul {
		display: flex;
		flex-direction: column;
		align-items: stretch;
		width: 100%;
		margin: 0;
		padding: 0;
	}

	.sidebar li {
		width: 100%;
		padding: calc(var(--pico-spacing) * 0.25) 0;
		list-style: none;
	}

	.sidebar img {
		display: block;
		margin: 0 auto;
	}

	#main-content {
		flex: 1;
		/* Remove top padding and centered margins */
		padding-top: 0;
		margin-left: 0;
		margin-right: 0;
		/* Added padding to the right side */
		padding-right: var(--pico-spacing);
	}

	/* Mobile Responsiveness */
	@media (max-width: 768px) {
		.layout {
			flex-direction: column;
		}

		.sidebar {
			width: 100%;
			height: auto;
			position: relative;
			border-right: none;
			border-bottom: var(--pico-border-width) solid var(--pico-muted-border-color);
		}

		#main-content {
			padding-right: 0;
		}
	}
</style>
