// A file dragged over the window selects its mode and shows Display modes while it is over the window; leaving puts
// back the page and mode, dropping keeps Display modes (2026-10-14). Studio simulates the OS drag events (testDrag).
window.ttlcd.page('dashboard');
await T.wait(500);
const S = () => T.store().S.cfg;
const modeBefore = S().Mode;
const drag = (a) => T.send({ t: 'testDrag', a, paths: ['/nonexistent/picture.png'] });
drag('enter');
await T.wait(500);
T.ok('while a picture is dragged over, Display modes shows', T.store().page === 'modes', `page: ${T.store().page}`);
T.ok('while a picture is dragged over, the Image mode is selected', S().Mode !== modeBefore || String(modeBefore).includes('Image'),
  `mode before: ${modeBefore}, now: ${S().Mode}`);
T.ok('the drop hint shows', T.store().dnd.active === true);
drag('leave');
await T.wait(500);
T.ok('leaving puts the page back', T.store().page === 'dashboard', `page: ${T.store().page}`);
T.ok('leaving puts the mode back', S().Mode === modeBefore, `mode: ${S().Mode}, before: ${modeBefore}`);
T.ok('leaving hides the drop hint', T.store().dnd.active === false);
drag('enter');
await T.wait(300);
drag('drop');
await T.wait(500);
T.ok('dropping keeps Display modes', T.store().page === 'modes', `page: ${T.store().page}`);
