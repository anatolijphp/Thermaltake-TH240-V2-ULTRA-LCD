// Builds the Studio UI into <outdir> (default dist/): index.html, app.js (React + components, minified) and
// app.css (every component's .scss, compiled by Sass and bundled in import order).
//   node build.mjs [outdir]
// The studio crate's build.rs runs it with the Node.js from tools/node, so `cargo build` always embeds a fresh UI.
import * as esbuild from 'esbuild';
import * as sass from 'sass';
import { copyFileSync, mkdirSync } from 'node:fs';
import { dirname, join, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';

const here = dirname(fileURLToPath(import.meta.url));
const out = resolve(process.argv[2] || join(here, 'dist'));
mkdirSync(out, { recursive: true });

// .scss imports from components: compiled with Sass (shared tokens/mixins via `@use 'tokens'`)
const scss = {
  name: 'scss',
  setup(b) {
    b.onLoad({ filter: /\.scss$/ }, (args) => {
      const r = sass.compile(args.path, { loadPaths: [join(here, 'src', 'styles')], style: 'expanded', sourceMap: false });
      return { contents: r.css, loader: 'css', watchFiles: r.loadedUrls.map((u) => fileURLToPath(u)) };
    });
  },
};

await esbuild.build({
  entryPoints: { app: join(here, 'src', 'main.jsx') },
  bundle: true,
  outdir: out,
  format: 'iife',
  target: ['chrome110', 'safari16'], // WebView2 (Chromium) and WebKitGTK 2.4x
  jsx: 'automatic',
  minify: true,
  legalComments: 'none',
  define: { 'process.env.NODE_ENV': '"production"' },
  plugins: [scss],
  logLevel: 'warning',
});
copyFileSync(join(here, 'index.html'), join(out, 'index.html'));
