import assert from "node:assert/strict";
import test from "node:test";
import handler from "../src/index.mjs";

const valid = "/releases/canary/0.1.1/MikeyTechLab-0.1.1-signed.bin";
const manifest = "/releases/canary/0.1.1/manifest-0.1.1.json";
const latest = "/channels/canary/latest.json";
const fixture = new Uint8Array([0xE9, 0x01, 0x02]);
function makeEnv(calls) {
  return { OTA_ASSETS: { async get(key) {
    calls.push(key);
    return { body: fixture, size: fixture.length, httpEtag: '"fixture"' };
  } } };
}
function req(path, method="GET") { return new Request("https://mikey-ota.example.workers.dev" + path, { method }); }

test("health endpoint returns no secret state", async () => {
  const r = await handler.fetch(req("/healthz"), {});
  assert.equal(r.status, 200);
  assert.equal(await r.text(), "mikey-ota-gateway-ready\n");
  assert.equal(r.headers.get("Cache-Control"), "no-store");
});
test("allowed release binary GET is read-only", async () => {
  const calls=[];
  const r=await handler.fetch(req(valid), makeEnv(calls));
  assert.equal(r.status,200);
  assert.deepEqual(calls,[valid.slice(1)]);
  assert.equal(r.headers.get("content-type"),"application/octet-stream");
  assert.equal(r.headers.get("cache-control"),"public, max-age=31536000, immutable");
  assert.deepEqual(new Uint8Array(await r.arrayBuffer()), fixture);
});
test("allowed HEAD returns headers but not body", async () => {
  const calls=[];
  const r=await handler.fetch(req(manifest,"HEAD"), makeEnv(calls));
  assert.equal(r.status,200);
  assert.equal(await r.text(),"");
  assert.deepEqual(calls,[manifest.slice(1)]);
});
test("channel pointer is untrusted discovery with no-store", async () => {
  const r=await handler.fetch(req(latest),makeEnv([]));
  assert.equal(r.status,200);
  assert.equal(r.headers.get("cache-control"),"no-store");
});
test("all write methods fail even if bucket has write capability", async () => {
  for (const method of ["PUT","POST","DELETE","PATCH"]) {
    const r=await handler.fetch(req(valid,method),makeEnv([]));
    assert.equal(r.status,405);
  }
});
test("reject traversal, cross-paths, malformed versions, querystrings", async () => {
  const bad=[
    "/releases/canary/0.1.1/MikeyTechLab-0.1.2-signed.bin",
    "/releases/canary/0.1.1/manifest-0.1.2.json",
    "/releases/canary/v0.1.1/manifest-0.1.1.json",
    "/releases/other/0.1.1/manifest-0.1.1.json",
    "/releases/canary/0.1.1/other.bin",
    "/channels/other/latest.json",
    "/keys/private-key.pem",
    "/healthz/extra",
    "/releases/canary/0.1.1/manifest-0.1.1.json?q=1",
    "/releases//canary/0.1.1/manifest-0.1.1.json",
    "/releases/canary/%30.1.1/manifest-0.1.1.json",
  ];
  const calls=[];
  for (const path of bad) {
    const r=await handler.fetch(req(path),makeEnv(calls));
    assert.equal(r.status,404,path);
  }
  assert.deepEqual(calls,[]);
});
test("missing object returns 404", async () => {
  const r=await handler.fetch(req(valid),{OTA_ASSETS:{async get(){return null;}}});
  assert.equal(r.status,404);
});
test("missing binding fails closed", async () => {
  const r=await handler.fetch(req(valid),{});
  assert.equal(r.status,503);
});
