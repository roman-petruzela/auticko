// Rozpracováno: zatím jen spojení a výpis zpráv. Ovládání (drive) přibude podle docs/api.md.
const ws = new WebSocket("ws://192.168.4.1:81");
ws.onopen = () => (document.getElementById("stav").textContent = "připojeno");
ws.onclose = () => (document.getElementById("stav").textContent = "odpojeno");
ws.onmessage = (e) => (document.getElementById("data").textContent = e.data);
