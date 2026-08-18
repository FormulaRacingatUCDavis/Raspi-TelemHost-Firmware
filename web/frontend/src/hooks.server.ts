import type { Handle } from '@sveltejs/kit';

const BACKEND_URL = 'http://backend:8000';

export const handle: Handle = async ({ event, resolve }) => {
    if (event.url.pathname.startsWith('/api/')) {
        const url = `${BACKEND_URL}${event.url.pathname}${event.url.search}`;

        const response = await fetch(url, {
            method: event.request.method,
            headers: event.request.headers,
            body:
                event.request.method === 'GET' ||
                event.request.method === 'HEAD'
                    ? undefined
                    : await event.request.arrayBuffer()
        });

        return new Response(response.body, {
            status: response.status,
            statusText: response.statusText,
            headers: response.headers
        });
    }

    return resolve(event);
};