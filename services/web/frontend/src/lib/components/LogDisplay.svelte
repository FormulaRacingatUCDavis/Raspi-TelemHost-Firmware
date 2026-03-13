<script lang="ts">
    import { onMount } from "svelte";

    let { source, log, logPath } = $props();
	type Log = { file_name: string; creation_date: number; file_size: number };
	let files = $state<Log[]>([]);

    function formatTime(ts: number) {
    return new Date(ts * 1000).toLocaleString();
    }

    function formatSize(bytes: number) {
    if (bytes < 1024) return `${bytes} B`;
    if (bytes < 1024 * 1024) return `${(bytes / 1024).toFixed(2)} KB`;
    return `${(bytes / 1024 / 1024).toFixed(2)} MB`;
    }

	async function updateDropdown() {
		const res = await fetch(`/api/${source}/logs/list/${logPath}`);
		const data: Log[] = await res.json();
		files = data;
	}
    
    onMount(() => {
        updateDropdown();
    });
</script>

<article data-theme="light">
	<select bind:value={log} onfocus={updateDropdown}>
		<option selected disabled value="">Select</option>
		{#if files.length > 0}
			{#each files as file}
				<option value={file.file_name}>
					{file.file_name}
				</option>
			{/each}
		{/if}
	</select>
	<hr />
	<small>
		<div class="overflow-auto" style="max-height: 300px;">
            <table>
            <thead>
                <tr>
                <th>Filename</th>
                <th>Date Created</th>
                <th>Size</th>
                </tr>
            </thead>
            <tbody>
                {#each files as file}
                <tr>
                    <th scope="row">{file.file_name}</th>
                    <td>{formatTime(file.creation_date)}</td>
                    <td>{formatSize(file.file_size)}</td>
                </tr>
                {/each}
            </tbody>
            </table>
		</div>
	</small>
</article>
