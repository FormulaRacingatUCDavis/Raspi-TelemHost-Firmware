const Camera = () => {
    const main = document.getElementById("main-content");
    main.innerHTML = `
        <article data-theme="light">
            <div class="grid">
                <img id="frame" src="images/photo_error.jpg" width="100%" />
            </div>
        </article>
        <div style="text-align:center;">
            <h3 id="statusTextArea"></h3>
        </div>
    `;
}

export default Camera;