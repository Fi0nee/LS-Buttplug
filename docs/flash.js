async function connectAndFlash() {
  const board = document.getElementById("boardSelect").value;
  const port = await navigator.serial.requestPort();
  await port.open({ baudRate: 115200 });

  const firmwareFiles = [
    `firmware/${board}/bootloader-${board}.bin`,
    `firmware/${board}/partitions-${board}.bin`,
    `firmware/${board}/firmware-${board}.bin`
  ];

  for (let file of firmwareFiles) {
    const response = await fetch(file);
    const data = await response.arrayBuffer();
    // здесь код записи data на ESP32 через port.writable
    console.log(`Flashing ${file}, size: ${data.byteLength}`);
  }

  alert("Flashing complete!");
}
