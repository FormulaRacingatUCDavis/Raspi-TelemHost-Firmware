<script lang="ts">
	import '@picocss/pico/css/pico.css';
	let { children } = $props();
</script>

<header class="container-fluid">
	<nav>
		<ul>
			<li><a href="/"><img src="/logo.svg" alt="FRUCD Logo" style="height:50px; width:auto;" /></a></li>
		</ul>
		<ul>
			<li><a href="/">Live</a></li>
			<li><a href="/logs">Logs</a></li>
		</ul>
	</nav>
</header>
<hr />
<main id="main-content" class="container-fluid">
	{@render children()}
</main>
