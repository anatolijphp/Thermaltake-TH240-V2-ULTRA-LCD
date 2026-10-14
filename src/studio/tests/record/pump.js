// docs/media/pump.gif: Display modes, the Pump tile chosen, the pump spinning at the CPU fan's speed, then set by hand
// from slow to fast, a different coolant colour. Recorded with fresh settings (nothing of yours is used or saved):
//   XDG_CONFIG_HOME=$(mktemp -d) TTLCD_NO_DEVICE=1 build/target/x86_64-unknown-linux-gnu/release/ttlcd-studio \
//     --record docs/media/pump.gif 10.5 src/studio/tests/record/pump.js
window.ttlcd.page('modes');
T.send({ t: 'set', k: 'Mode', v: 'Clock' });
await T.wait(1100);
T.send({ t: 'set', k: 'Mode', v: 'Pump' }); // the Pump tile
await T.wait(2300);
T.send({ t: 'set', k: 'PumpFan', v: 'manual' }); // Speed from: Set by hand
T.send({ t: 'set', k: 'PumpRpm', v: 600 });
await T.wait(1000);
for (let rpm = 800; rpm <= 3200; rpm += 200) { // drag the slider up
  T.send({ t: 'set', k: 'PumpRpm', v: rpm });
  await T.wait(140);
}
await T.wait(1200);
T.send({ t: 'set', k: 'PumpColor', v: 0x2BE36B }); // coolant: green
await T.wait(1400);
T.send({ t: 'set', k: 'PumpColor', v: 0x00D1FF });
