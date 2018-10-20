const cp = require('child_process')

let child

function pulse () {
  child = cp.execFile('./led', ['1', 'ff', '00', '00', '0', '10', '-d'], (err, stdout, stderr) => {
    if (err) console.log(err)
    console.log(stdout)
  })
}

pulse()

setTimeout(() => child.kill(), 1000)
