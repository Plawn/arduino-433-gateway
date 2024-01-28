Bun.serve({
    port: 3444,
    async fetch(req: Request) {
        console.log(req);
        console.log(await req.json());
        return new Response(`Bun!`);
    },
});