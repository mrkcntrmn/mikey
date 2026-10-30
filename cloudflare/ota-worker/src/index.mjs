// MIKEY-011 draft only. Public read-only HTTPS gateway; NO deploy workflow.
// Authentication comes from signed metadata and firmware verified ON DEVICE.
const releaseRe = /^\/releases\/(canary|stable)\/(\d+\.\d+\.\d+)\/(MikeyTechLab-(\d+\.\d+\.\d+)-signed\.bin|manifest-(\d+\.\d+\.\d+)\.json)$/;
const pointerRe = /^\/channels\/(canary|stable)\/latest\.json$/;

function getAllowedObjectKey(url) {
  if (url.search || url.hash || url.pathname.length > 160 ||
      url.pathname.includes("%") || url.pathname.includes("\\") ||
      url.pathname.includes("//")) return null;
  const release = releaseRe.exec(url.pathname);
  if (release && release[2] === (release[4] || release[5])) {
    return url.pathname.slice(1);
  }
  if (pointerRe.test(url.pathname)) return url.pathname.slice(1);
  return null;
}

export default {
  async fetch(request, env) {
    if (request.method !== "GET" && request.method !== "HEAD") {
      return new Response(null, { status: 405, headers: { Allow: "GET, HEAD" } });
    }

    let url;
    try {
      url = new URL(request.url);
    } catch {
      return new Response(null, { status: 400 });
    }

    if (url.pathname === "/healthz") {
      return new Response(request.method === "HEAD" ? null : "mikey-ota-gateway-ready\n", {
        headers: { "Content-Type": "text/plain; charset=utf-8", "Cache-Control": "no-store",
          "X-Content-Type-Options": "nosniff" },
      });
    }

    const key = getAllowedObjectKey(url);
    if (!key) return new Response(null, { status: 404, headers: { "Cache-Control": "no-store" } });
    if (!env?.OTA_ASSETS?.get) return new Response(null, { status: 503 });

    const object = await env.OTA_ASSETS.get(key);
    if (!object) return new Response(null, { status: 404, headers: { "Cache-Control": "no-store" } });

    const isBinary = key.endsWith(".bin");
    const isPointer = key.startsWith("channels/");
    const headers = new Headers({
      "Content-Type": isBinary ? "application/octet-stream" : "application/json; charset=utf-8",
      "Cache-Control": isPointer ? "no-store" : "public, max-age=31536000, immutable",
      "X-Content-Type-Options": "nosniff",
      "Content-Length": String(object.size),
    });
    if (object.httpEtag) headers.set("ETag", object.httpEtag);
    return new Response(request.method === "HEAD" ? null : object.body,
      { status: 200, headers });
  },
};
