// TT LCD Studio page: React components drawing what the Rust side sends (bridge.js). The same page runs in WebView2
// (Windows) and WebKitGTK (Linux).
import { createRoot } from 'react-dom/client';
import './styles/global.scss';
import { send } from './bridge.js';
import App from './App.jsx';

createRoot(document.getElementById('root')).render(<App />);
send({ t: 'ready' });
