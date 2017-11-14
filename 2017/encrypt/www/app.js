let express             = require('express');
let app                 = express();
let fs                  = require('fs');
let btoa                = require('btoa');
let atob                = require('atob');
let mem;
let buffer;
let _data;
let _pass;
let importObject = {
    env : {
        grow(needed) {
            end = mem.buffer.byteLength;
            mem.grow(needed);
            buffer = new Uint8Array(mem.buffer);
            return end;
        },
        read_file(filename, addr , size) {
            let filenameBuffer = '';
            while (buffer[filename] != 0) {
                filenameBuffer += String.fromCharCode(buffer[filename++]);
            }
            content = fs.readFileSync(filenameBuffer);
            for (i=0; i < content.length && size--;i++) {
                buffer[addr++] = content[i];
            }
        },
        read_data(addr) {
            for (i in _data) {
                buffer[addr++] = _data.charCodeAt(i);
            }
            return _data.length;
        },
        read_pass(addr) {
            for (i in _pass) {
                buffer[addr++] = _pass.charCodeAt(i);
            }
        },
        read_random(addr) {
            random_s = "";
            for(i = 0;i < 16;i++) {
                random_s += String.fromCharCode(Math.floor(Math.random()*256));
            }
            for (i in random_s) {
                buffer[addr++] = random_s.charCodeAt(i);
            }
        }
    }
};

function encrypt() {
    let wasmFile = fs.readFileSync('crypt.wasm');
    let r = WebAssembly.instantiate(wasmFile,importObject).then(
        (result) => {
            doEncrypt = result.instance.exports.encrypt;
            outSize = result.instance.exports.out_size;
            mem = result.instance.exports.memory;
            buffer = new Uint8Array(mem.buffer);
            pos = doEncrypt();
            let arr = buffer.subarray(pos,pos+outSize());
            r = String.fromCharCode.apply(null,arr);
            return btoa(r);
    });
    return r;
}

function decrypt() {
    let wasmFile = fs.readFileSync('crypt.wasm');
    let r = WebAssembly.instantiate(wasmFile,importObject).then(
        (result) => {
            doDecrypt = result.instance.exports.decrypt;
            outSize = result.instance.exports.out_size;
            mem = result.instance.exports.memory;
            buffer = new Uint8Array(mem.buffer);
            pos = doDecrypt();
            let arr = buffer.subarray(pos,pos+outSize());
            r = String.fromCharCode.apply(null,arr);
            return r;
        });
    return r;
}




app.get("/", function(req,res) {
    res.sendfile('public/index.html');
});

app.get("/www.zip",function(req,res) {
    res.sendfile('public/www.zip')
});

app.get("/background.jpg",function (req,res) {
    res.sendfile('public/background.jpg')
});

app.get('/encrypt', function(req,res) {
        _data = req.query['data'];
        _pass = req.query['pass'];
        _data = atob(_data);
        if(_data.length >= 0x1000) {
            return res.send("data to long");
        }
        if(_pass.length !== 8) {
            return res.send("please input 8 bytes password");
        }
        if(_data === "" || _pass === "") {
            return res.send("error");
        }
        encrypt().then(
            (r) => {
                res.send(r);
            }
        );
    }
);

app.get('/decrypt', function(req,res) {
        _data = req.query['data'];
        _pass = req.query['pass'];
        _data = atob(_data);
        if(_data.length >= 0x1000 || _data.length < 16 ) {
            return res.send("data length error");
        }
        if(_pass.length !== 8) {
            return res.send("please input 8 bytes password");
        }
        if(_data === "" || _pass === "") {
            return res.send("error");
        }
        decrypt().then(
            (r) => {
                res.send(r);
            }
        );
    }
);

app.listen(12345);