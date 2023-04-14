const wordsList = require('./words.js');


/*
* Created by Akshar Desai on 5/6/2022.
* Updated in April 2023.
*/

const alertContainer = document.querySelector("[data-alert-container]")
const guess_grid = document.querySelector("[data-guess-grid]")
const targetWords = require('./targetWords.js')
var guess_count = 1;
var win = false;
var lose = false;
var isClosing = false;
const { clipboard } = require('electron');
const startData = {
    "day": 0,
    "guess_1": 0,
    "guess_2": 0,
    "guess_3": 0,
    "guess_4": 0,
    "guess_5": 0,
    "guess_6": 0,
    "guess_7": 0,
    "guess_8": 0,
    "lose": false,
    "max_streak": 0,
    "percent": 0,
    "played": false,
    "played_count": 0,
    "played_letter": [
        [],
        [],
        [],
        [],
        [],
        [],
        [],
        []
    ],
    "streak": 0,
    "win": false
}
var gameData;
let nex_tim = document.getElementById("nex");
const offsetFromDate = new Date(2023, 3, 9)
const msOffset = Date.now() - offsetFromDate
const dayOffset = Math.floor(msOffset / 1000 / 60 / 60 / 24)
var nextDay = new Date();
nextDay.setDate(nextDay.getDate() + 1);
nextDay.setMinutes(0);
nextDay.setSeconds(0);
nextDay.setHours(0);
const targetWord = targetWords[dayOffset]
const stats_screen = document.getElementById("stats_screen");
const s2 = document.getElementById("s2")
const played_div = document.querySelector("#stat-container-played #stat");
const per_div = document.querySelector("#stat-container-win-per #stat");
const ms_div = document.querySelector("#stat-container-max-streak #stat");
const cs_div = document.querySelector("#stat-container-cur-streak #stat");
const distrib_div = document.querySelector("#distribs");
var hasPressedWipe = false;
document.getElementById("stats-button").onclick = showStats;
// document.getElementById("wipe").onclick = wipeSave;
stats_screen.addEventListener("click", hideStats);
s2.addEventListener('click', e => {
    e.stopImmediatePropagation()
})
var share_btn = document.getElementById("share");
share_btn.onclick = copyToClipboard;

const { ipcRenderer } = require('electron')

// Listen for the IPC message
ipcRenderer.on('saveType', (event, name, arg) => {
    // Call the function with the passed arguments
    setType(name, arg)
})
ipcRenderer.on('getType', (event, name, type_name) => {
    if (localStorage.getItem(type_name) == null) {
        return null;
    }
    ipcRenderer.send(name, JSON.parse(localStorage.getItem(type_name)));
});
var clicks = 0;
document.querySelector("#title-bar").addEventListener("click", () => {
    clicks++;
    if (clicks == 2) {
        console.log("double click");
        ipcRenderer.send("toggleMaximize");
        clicks = 0;
    }
    setTimeout(() => {
        clicks = 0;
    }, 250);
});


var played, win_per, cur_strk, max_strk, guess_1, guess_2, guess_3, guess_4, guess_5, guess_6, guess_7, guess_8, has_played;
played = 0;
win_per = 0;
cur_strk = 0;
max_strk = 0;
guess_1 = 0;
guess_2 = 0;
guess_3 = 0;
guess_4 = 0;
guess_5 = 0;
guess_6 = 0;
guess_7 = 0;
guess_8 = 0;
has_played = false;
var guess_grid_json = [
    [],
    [],
    [],
    [],
    [],
    [],
    [],
    [],
]

function getKey(keyName) {
    return document.querySelector(`[data-key="${keyName}"]`);
}

function getLengthOfJson() {
    var length = 0;
    for (let i = 0; i < 8; i++) {
        if (guess_grid_json[i].length > 0) {
            length++;
        }
    }
    return length;
}

function wipeSave() {
    if (!hasPressedWipe) {
        hasPressedWipe = true;
        showAlert("Click again to confirm wipe.", 4000, () => { hasPressedWipe = false }, -1);
    } else {
        localStorage.removeItem("gameData");
        hideStats();
        hasPressedWipe = false;
        window.location.reload();
    }


}

function updateTimer() {
    const now = new Date();
    const diff = nextDay.getTime() - now.getTime();
    const hours = Math.floor(diff / (1000 * 60 * 60)).toString().padStart(2, '0');
    const minutes = Math.floor((diff / (1000 * 60)) % 60).toString().padStart(2, '0');
    const seconds = Math.floor((diff / 1000) % 60).toString().padStart(2, '0');




    if (hours == "00" && minutes == "00" && seconds == "00") {
        clearInterval(timerUpdate)
        window.location.reload();
    };
    nex_tim.textContent = `${hours}:${minutes}:${seconds}`;
}

var timerUpdate = setInterval(updateTimer, 1000);

function removeByIndex(str, index) {
    return str.slice(0, index) + str.slice(index + 1);
}

function calculateResult(index, guess) {
    var temp = targetWord;
    var out = [];
    // check correct letters
    for (let i = 0; i < 6; i++) {
        if (targetWord[i] == guess[i]) {
            out[i] = "correct";
            temp = removeByIndex(temp, temp.indexOf(guess[i]));
        }
    }
    for (let i = 0; i < 6; i++) {
        if (temp.includes(guess[i]) && out[i] != "correct") {
            out[i] = "wrong-location";
            temp = removeByIndex(temp, temp.indexOf(guess[i]));
        }
        else if (out[i] != "correct") {
            out[i] = "wrong";
        }

    }
    if (out[index] == "correct") {
        getKey(guess[index].toUpperCase()).classList.add("correct");
    }
    else if (out[index] == "wrong-location") {
        getKey(guess[index].toUpperCase()).classList.add("wrong-location");
    }
    else if (out[index] == "wrong") {
        getKey(guess[index].toUpperCase()).classList.add("wrong");
    }

    return out[index];
}
const cb = () => {
    if (win || lose) {
        setTimeout(
            () => {
                if (!s2.classList.contains("hide")) {
                    showStats();
                }
            }, 400)
    }
}

function importData() {
    gameData = localStorage.getItem("gameData") ? JSON.parse(localStorage.getItem("gameData")) : startData
    played = gameData.played_count;
    has_played = gameData.played;
    win_per = gameData.percent;
    cur_strk = gameData.streak;
    max_strk = gameData.max_streak;
    guess_1 = gameData.guess_1;
    guess_2 = gameData.guess_2;
    guess_3 = gameData.guess_3;
    guess_4 = gameData.guess_4;
    guess_5 = gameData.guess_5;
    guess_6 = gameData.guess_6;
    guess_7 = gameData.guess_7;
    guess_8 = gameData.guess_8;
    win = gameData.win;
    lose = gameData.lose;
    // been offline for more than one day.
    if (dayOffset > gameData.day + 1) {
        if (cur_strk > max_strk) {
            max_strk = cur_strk;
        }
        cur_strk = 0;
        win = false;
        lose = false;
        has_played = false;
        guess_grid_json = [
            [], [], [], [], [], [], [], []
        ];
        exportData();
    }
    // reset board if next day
    else if (dayOffset > gameData.day) {
        win = false;
        lose = false;
        has_played = false;
        guess_grid_json = [
            [], [], [], [], [], [], [], []
        ];

        exportData();
    }
    if (gameData.played) {
        guess_grid_json = gameData.played_letter;
        var length = getLengthOfJson();
        guess_count = getLengthOfJson() + 1;
        if (length >= 1) {
            let g = guess_grid_json[0][0] + guess_grid_json[0][1] + guess_grid_json[0][2] + guess_grid_json[0][3] + guess_grid_json[0][4] + guess_grid_json[0][5];

            for (let i = 0; i < 6; i++) {
                guess_grid.children[i].dataset.letter = guess_grid_json[0][i];
                guess_grid.children[i].textContent = guess_grid_json[0][i];
                guess_grid.children[i].style.transition = 'transform 100ms linear;';
                guess_grid.children[i].addEventListener("transitionend", () => {
                    guess_grid.children[i].classList.remove("fast");
                    guess_grid.children[i].dataset.state = calculateResult(i, g);
                    guess_grid.children[i].style.transition = 'transform 250ms linear;';
                }, { once: true });
            }
        }
        if (length >= 2) {

            let g = guess_grid_json[1][0] + guess_grid_json[1][1] + guess_grid_json[1][2] + guess_grid_json[1][3] + guess_grid_json[1][4] + guess_grid_json[1][5];

            for (let i = 6; i < 12; i++) {
                guess_grid.children[i].dataset.letter = guess_grid_json[1][i - 6];
                guess_grid.children[i].textContent = guess_grid_json[1][i - 6];
                guess_grid.children[i].style.transition = 'transform 100ms linear;';
                guess_grid.children[i].addEventListener("transitionend", () => {
                    guess_grid.children[i].classList.remove("fast");
                    guess_grid.children[i].dataset.state = calculateResult(i - 6, g);
                    guess_grid.children[i].style.transition = 'transform 250ms linear;';
                }, { once: true });
            }

        }

        if (length >= 3) {
            let g = guess_grid_json[2][0] + guess_grid_json[2][1] + guess_grid_json[2][2] + guess_grid_json[2][3] + guess_grid_json[2][4] + guess_grid_json[2][5];

            for (let i = 12; i < 18; i++) {
                guess_grid.children[i].dataset.letter = guess_grid_json[2][i - 12];
                guess_grid.children[i].textContent = guess_grid_json[2][i - 12];
                guess_grid.children[i].style.transition = 'transform 100ms linear;';
                guess_grid.children[i].addEventListener("transitionend", () => {
                    guess_grid.children[i].classList.remove("fast");
                    guess_grid.children[i].dataset.state = calculateResult(i - 12, g);
                    guess_grid.children[i].style.transition = 'transform 250ms linear;';
                }, { once: true });
            }
        }
        if (length >= 4) {
            let g = guess_grid_json[3][0] + guess_grid_json[3][1] + guess_grid_json[3][2] + guess_grid_json[3][3] + guess_grid_json[3][4] + guess_grid_json[3][5];

            for (let i = 18; i < 24; i++) {
                guess_grid.children[i].dataset.letter = guess_grid_json[3][i - 18];
                guess_grid.children[i].textContent = guess_grid_json[3][i - 18];
                guess_grid.children[i].style.transition = 'transform 100ms linear;';
                guess_grid.children[i].addEventListener("transitionend", () => {
                    guess_grid.children[i].classList.remove("fast");
                    guess_grid.children[i].dataset.state = calculateResult(i - 18, g);
                    guess_grid.children[i].style.transition = 'transform 250ms linear;';
                }, { once: true });
            }
        }
        if (length >= 5) {
            let g = guess_grid_json[4][0] + guess_grid_json[4][1] + guess_grid_json[4][2] + guess_grid_json[4][3] + guess_grid_json[4][4] + guess_grid_json[4][5];

            for (let i = 24; i < 30; i++) {
                guess_grid.children[i].dataset.letter = guess_grid_json[4][i - 24];
                guess_grid.children[i].textContent = guess_grid_json[4][i - 24];
                guess_grid.children[i].style.transition = 'transform 100ms linear;';
                guess_grid.children[i].addEventListener("transitionend", () => {
                    guess_grid.children[i].classList.remove("fast");
                    guess_grid.children[i].dataset.state = calculateResult(i - 24, g);
                    guess_grid.children[i].style.transition = 'transform 250ms linear;';
                }, { once: true });
            }
        }
        if (length >= 6) {
            let g = guess_grid_json[5][0] + guess_grid_json[5][1] + guess_grid_json[5][2] + guess_grid_json[5][3] + guess_grid_json[5][4] + guess_grid_json[5][5];

            for (let i = 30; i < 36; i++) {
                guess_grid.children[i].dataset.letter = guess_grid_json[5][i - 30];
                guess_grid.children[i].textContent = guess_grid_json[5][i - 30];
                guess_grid.children[i].style.transition = 'transform 100ms linear;';
                guess_grid.children[i].addEventListener("transitionend", () => {
                    guess_grid.children[i].classList.remove("fast");
                    guess_grid.children[i].dataset.state = calculateResult(i - 30, g);
                    guess_grid.children[i].style.transition = 'transform 250ms linear;';
                }, { once: true });
            }
        }
        if (length >= 7) {
            let g = guess_grid_json[6][0] + guess_grid_json[6][1] + guess_grid_json[6][2] + guess_grid_json[6][3] + guess_grid_json[6][4] + guess_grid_json[6][5];

            for (let i = 36; i < 42; i++) {
                guess_grid.children[i].dataset.letter = guess_grid_json[6][i - 36];
                guess_grid.children[i].textContent = guess_grid_json[6][i - 36];
                guess_grid.children[i].style.transition = 'transform 100ms linear;';
                guess_grid.children[i].addEventListener("transitionend", () => {
                    guess_grid.children[i].classList.remove("fast");
                    guess_grid.children[i].dataset.state = calculateResult(i - 36, g);
                    guess_grid.children[i].style.transition = 'transform 250ms linear;';
                }, { once: true });
            }
        }
        if (length >= 8) {
            let g = guess_grid_json[7][0] + guess_grid_json[7][1] + guess_grid_json[7][2] + guess_grid_json[7][3] + guess_grid_json[7][4] + guess_grid_json[7][5];

            for (let i = 42; i < 48; i++) {
                guess_grid.children[i].dataset.letter = guess_grid_json[7][i - 42];
                guess_grid.children[i].textContent = guess_grid_json[7][i - 42];
                guess_grid.children[i].style.transition = 'transform 100ms linear;';
                guess_grid.children[i].addEventListener("transitionend", () => {
                    guess_grid.children[i].classList.remove("fast");
                    guess_grid.children[i].dataset.state = calculateResult(i - 42, g);
                    guess_grid.children[i].style.transition = 'transform 250ms linear;';
                }, { once: true });
            }
        }

        setTimeout(() => {
            for (let i = 0; i < length * 6; i++) {
                setTimeout(() => {
                    guess_grid.children[i].classList.add("fast");
                }, i * 25);
            }
        }, 400);
        guess_grid.children[(length * 6) - 1].addEventListener("transitionend", () => {
            setTimeout(() => {
                const tiles = Array.from(guess_grid.children).slice((length - 1) * 6, length * 6);
                const guess = tiles.reduce((word, tile) => {
                    return word + tile.dataset.letter
                }, "")
                if (gameData.win || gameData.lose) {
                    checkWinLose(guess, tiles, true, cb);
                }
            }, 300);
        }, { once: true });


    }


}

function exportData() {
    gameData.played_count = played;
    gameData.percent = win_per;
    gameData.streak = cur_strk;
    gameData.max_streak = max_strk;
    gameData.guess_1 = guess_1;
    gameData.guess_2 = guess_2;
    gameData.guess_3 = guess_3;
    gameData.guess_4 = guess_4;
    gameData.guess_5 = guess_5;
    gameData.guess_6 = guess_6;
    gameData.guess_7 = guess_7;
    gameData.guess_8 = guess_8;
    gameData.day = dayOffset;
    gameData.played_letter = guess_grid_json;
    gameData.win = win;
    gameData.lose = lose;
    gameData.played = has_played;
    localStorage.setItem("gameData", JSON.stringify(gameData));
}



function showStats() {
    s2.classList.remove("hide");
    s2.style.display = "block";
    stats_screen.style.display = "flex";
}






function calculateWinPercentage() {
    var total = played;
    var correct = guess_1 + guess_2 + guess_3 + guess_4 + guess_5 + guess_6 + guess_7 + guess_8;
    if (total == 0) {
        return 0;
    }
    var win_per = Math.round((correct / total) * 100);
    return win_per;
}

function updateStats() {
    played_div.textContent = played;
    per_div.textContent = win_per + "%";
    ms_div.textContent = max_strk;
    cs_div.textContent = cur_strk;
    if (win || lose) {
        document.getElementById("foot").style.display = "";
    }
    else {
        document.getElementById("foot").style.display = "none";
    }
    distrib_div.children[0].textContent = '1: ' + guess_1;
    distrib_div.children[1].textContent = '2: ' + guess_2;
    distrib_div.children[2].textContent = '3: ' + guess_3;
    distrib_div.children[3].textContent = '4: ' + guess_4;
    distrib_div.children[4].textContent = '5: ' + guess_5;
    distrib_div.children[5].textContent = '6: ' + guess_6;
    distrib_div.children[6].textContent = '7: ' + guess_7;
    distrib_div.children[7].textContent = '8: ' + guess_8;
    if (win) {
        distrib_div.children[guess_count - 2].style.color = "hsl(114, 42%, 59%)";
    }

}


function hideStats() {
    if (stats_screen.style.display == "none" || isClosing) {
        return;
    }
    s2.classList.add("hide");
    isClosing = true;
    s2.addEventListener("animationend", () => {
        setTimeout(() => {
            stats_screen.style.display = "none";
            isClosing = false;
        }, 200);
        s2.style.display = "none";
        s2.classList.remove("hide");
    }, { once: true });
}


function createOutput() {
    var output = "Sesordle ";
    output += dayOffset + " ";
    if (win) {
        output += (guess_count - 1) + "/8\n";
    }
    else if (lose) {
        output += "X/8\n";
    }
    var counter = 1;
    for (var i = 0; i < guess_grid.children.length; i++) {
        var tile = guess_grid.children[i];
        if (tile.dataset.state != undefined) {
            if (tile.dataset.state == "correct") {
                output += "🟩";
            } else if (tile.dataset.state == "wrong") {
                output += "⬛";
            } else if (tile.dataset.state == "wrong-location") {
                output += "🟨";
            }
        }
        if (counter == 6 && guess_grid.children[i + 1]) {
            if (guess_grid.children[i + 1].dataset.state != undefined) {
                output += "\n";
                counter = 1;
            }
        }
        else {
            counter++;
        }
    }

    return output;
}

function copyToClipboard() {
    clipboard.writeText(createOutput());
    showAlert("Copied to clipboard!", 1000, null, -1);
}

function startInteraction() {
    document.addEventListener("keydown", handleKeyPress);
}

function pressKey(key) {
    const activeTiles = getActiveTiles();
    if (activeTiles.length >= 6) return;
    const next = guess_grid.querySelector(":not([data-letter])");
    next.dataset.letter = key.toLowerCase();
    next.textContent = key;
    next.style.transition = "transform 50ms linear";
    next.classList.add("expand");
    next.addEventListener("transitionend", () => {
        next.classList.remove("expand");
        setTimeout(() => {
            next.style.transition = "transform 250ms linear";
        }, 110);
    }, { once: true });
    next.dataset.state = "active"
}

document.addEventListener("keydown", (e) => {
    if (e.key === "Escape") {
        hideStats();
        return;
    }
})


function handleKeyPress(e) {
    if (e.key === "Enter") {
        var activeTiles = getActiveTiles();
        for (let i = 0; i < activeTiles.length; i++) {
            const tile = activeTiles[i];
            if (tile.classList.contains("expand")) {
                tile.style.transition = "";
                tile.classList.remove("expand");
            }
        }
        setTimeout(() => {
            submitGuess();
        }, 250);
    }
    if (e.key === "Backspace" || e.key === "Delete") {
        deleteKey();
        return;
    }
    if (e.key.match(/^[a-zA-Z]$/)) {
        pressKey(e.key);
        return;
    }

}

function getActiveTiles() {
    return guess_grid.querySelectorAll("[data-state='active']")

}

function getGuessTiles() {
    return [guess_grid.children[(guess_count - 1) * 6], guess_grid.children[(guess_count - 1) * 6 + 1], guess_grid.children[(guess_count - 1) * 6 + 2], guess_grid.children[(guess_count - 1) * 6 + 3], guess_grid.children[(guess_count - 1) * 6 + 4], guess_grid.children[(guess_count - 1) * 6 + 5]];
}

function stopInteraction() {
    document.removeEventListener("keydown", handleKeyPress);
}

function deleteKey() {
    const activeTiles = getActiveTiles();
    const lastTile = activeTiles[activeTiles.length - 1];
    if (lastTile == null) return;
    lastTile.textContent = "";
    delete lastTile.dataset.letter;
    delete lastTile.dataset.state;
}

function submitGuess() {
    const activeTiles = [...getActiveTiles()]
    const guess = activeTiles.reduce((word, tile) => {
        return word + tile.dataset.letter
    }, "")
    if (activeTiles.length < 6) {
        if (alertContainer.children.length < 3) {
            showAlert("Not enough letters.");
        }
        shakeTiles([...getGuessTiles()]);
        return
    }
    if (wordsList.indexOf(guess) === -1) {
        if (alertContainer.children.length < 3) {
            showAlert("Not in word list.");
        }
        shakeTiles([...getGuessTiles()]);
        return;
    }
    stopInteraction()
    if (!has_played) {
        has_played = true;
    }
    activeTiles.forEach((...params) => {
        flipTiles(...params, guess)
    })
    guess_count++;

}






function showAlert(message, duration = 1000, callback = null, zIndex = 0) {
    const alert = document.createElement("div");
    if (zIndex == -1) {
        stats_screen.style.zIndex = 1;
    }
    alert.textContent = message;
    alert.style.zIndex = zIndex;
    alert.classList.add("alert");
    alertContainer.prepend(alert);
    if (duration == null) return
    if (duration != -1) {
        setTimeout(() => {
            alert.classList.add("hide")
            alert.addEventListener("transitionend", () => {
                alert.remove()
                if (alertContainer.length == 0) {
                    stats_screen.style.zIndex = 2;
                }
                if (callback) callback();
            });
        }, duration);
    }

}

function shakeTiles(tiles) {
    tiles.forEach(tile => {
        tile.classList.add("shake")
        tile.addEventListener("animationend", () => {
            tile.classList.remove("shake")
        }, { once: true });
    });
}

function flipTiles(tile, index, array, guess) {
    const letter = tile.dataset.letter;
    tile.style.transition = "";
    setTimeout(() => {
        tile.style.transition = "transform 250ms linear";
    }, 2);
    setTimeout(() => {
        tile.classList.add("flip")
    }, (index * 500) / 2);



    tile.addEventListener("transitionend", () => {
        tile.classList.remove("flip");
        tile.dataset.state = calculateResult(index, guess);



        if (index === array.length - 1) {
            tile.addEventListener("transitionend", () => {
                startInteraction()
                checkWinLose(guess, array);
            }, { once: true });
        }
    }, { once: true });
    guess_grid_json[guess_count - 1][index] = letter;



}

function checkWinLose(guess, tiles, imported = false, cb = null) {
    if (guess == targetWord) {
        document.querySelector("#main").innerHTML = resultedText
        switch (guess_count) {
            case 2:
                showAlert("Genius!", 1000, cb)
                if (!imported) {
                    guess_1++;
                }
                break;
            case 3:
                showAlert("Master!", 1000, cb)
                if (!imported) {
                    guess_2++;
                }
                break;
            case 4:
                showAlert("Expert!", 1000, cb)
                if (!imported) {
                    guess_3++;
                }
                break;
            case 5:
                showAlert("Pro!", 1000, cb)
                if (!imported) {
                    guess_4++;
                }
                break;
            case 6:
                showAlert("Well Done!", 1000, cb)
                if (!imported) {
                    guess_5++;
                }
                break;
            case 7:
                showAlert("Phew!", 1000, cb)
                if (!imported) {
                    guess_6++;
                }
                break;
            case 8:
                showAlert("Not Bad!", 1000, cb)
                if (!imported) {
                    guess_7++;
                }
                break;
            case 9:
                showAlert("Too Close!", 1000, cb)
                if (!imported) {
                    guess_8++;
                }
                break;
        }
        if (!imported) {
            win = true;
            played++
            cur_strk++;
            win_per = calculateWinPercentage();
            if (cur_strk > max_strk) {
                max_strk = cur_strk;
            }
            updateStats();
        }
    } else if (guess_count == 9) {
        showAlert(targetWord.toUpperCase(), 1000, cb, -1)
        document.querySelector("#main").innerHTML = resultedText
        if (!imported) {
            win = false;
            lose = true;
        }
        stopInteraction()
        setTimeout(() => {
            if (!s2.classList.contains("hide") && !imported) {
                showStats();
            }
            return
        }, 1700);
        if (!imported) {
            played++;
            cur_strk = 0;
            win_per = calculateWinPercentage();
            if (cur_strk > max_strk) {
                max_strk = cur_strk;
            }
            updateStats();
        }
        exportData();
        return;
    }
    if (win) {

        danceTiles(tiles)
        stopInteraction()
        setTimeout(() => {
            if (!s2.classList.contains("hide") && !imported) {
                showStats();
            }
            return
        }, 1400);
    }
    exportData();
}


function danceTiles(tiles) {
    tiles.forEach((tile, index) => {
        setTimeout(() => {
            tile.classList.add("dance")
            tile.addEventListener("animationend", () => {
                tile.classList.remove("dance")
            }, { once: true });
        }, (index * 500) / 5);
    });
}


function setType(name, data) {
    localStorage.setItem(name, JSON.stringify(data));
}


const baseText = `Sesordle — Day ${dayOffset}`
const resultedText = `${baseText}: <span style='font-weight: bold'>${targetWord.toUpperCase()}</span>`

document.querySelector("#main").innerHTML = baseText;
startInteraction();
importData();
if (win || lose) {
    stopInteraction();
    document.querySelector("#main").innerHTML = resultedText
}
updateStats();
