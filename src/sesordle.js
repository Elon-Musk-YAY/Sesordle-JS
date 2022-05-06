const _path = require('path');
const fs = require('fs');

const alertContainer = document.querySelector("[data-alert-container]")
const guess_grid = document.querySelector("[data-guess-grid]")
const targetWords = ["tromie", "boxers", "yeeter", "yeeter"]
var guess_count = 1;
var win = false;
var lose = false;
const {clipboard} = require('electron');
const json_file = require(`${__dirname}/data.json`);
let nex_tim = document.getElementById("nex");
let letterCount;
const offsetFromDate = new Date(2022,4,2)
const msOffset = Date.now() - offsetFromDate
const dayOffset = msOffset / 1000 / 60 / 60 / 24
var nextDay = new Date();
nextDay.setDate(nextDay.getDate() + 1);
nextDay.setMinutes(0);
nextDay.setSeconds(0);
nextDay.setHours(0);
const targetWord = targetWords[Math.floor(dayOffset)]
const stats_screen = document.getElementById("stats_screen");
const s2 = document.getElementById("s2");
const played_div = document.querySelector("#stat-container-played #stat");
const per_div = document.querySelector("#stat-container-win-per #stat");
const ms_div = document.querySelector("#stat-container-max-streak #stat");
const cs_div = document.querySelector("#stat-container-cur-streak #stat");
const distrib_div = document.querySelector("#distribs");
document.getElementById("stats-button").onclick = showStats;
stats_screen.addEventListener("click", hideStats);
var x, y;
var share_btn = document.getElementById("share");
share_btn.onclick = copyToClipboard;

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

function getLengthOfJson() {
    var length = 0;
    for (let i = 0; i<6; i++) {
        if (guess_grid_json[i].length > 0) {
            length++;
        }
    }
    return length;
}

function updateTimer () {
    var currentTime = new Date();
    var hours = nextDay.getHours() - (currentTime.getHours()-23);
    var minutes = nextDay.getMinutes() - (currentTime.getMinutes()-59);
    var seconds = nextDay.getSeconds() - (currentTime.getSeconds()-59);
    if (hours == 0 && minutes == 0 && seconds == 0) {
        clearInterval(timerUpdate)
    };
    if (seconds == 60) {
        seconds = 0;
        minutes ++;
    }
    if (minutes == 60) {
        minutes = 0;
        hours ++;
    }
    if (seconds < 10) {
        seconds = "0"+seconds;
    }
    if (minutes < 10) {
        minutes = "0"+minutes;
    }
    if (hours < 10) {
        hours = "0"+hours;
    }
    nex_tim.textContent = `${hours}:${minutes}:${seconds}`;
}

var timerUpdate = setInterval(updateTimer, 1000);

function importData () {
    if (json_file.played) {
        guess_grid_json = json_file.played_letter;
        var length = getLengthOfJson();
        if (length >= 1) {
            guess_grid.children[0].dataset.letter = guess_grid_json[0][0];
            guess_grid.children[0].textContent = guess_grid_json[0][0];

            guess_grid.children[1].dataset.letter = guess_grid_json[0][1];
            guess_grid.children[1].textContent = guess_grid_json[0][1];

            guess_grid.children[2].dataset.letter = guess_grid_json[0][2];
            guess_grid.children[2].textContent = guess_grid_json[0][2];

            guess_grid.children[3].dataset.letter = guess_grid_json[0][3];
            guess_grid.children[3].textContent = guess_grid_json[0][3];

            guess_grid.children[4].dataset.letter = guess_grid_json[0][4];
            guess_grid.children[4].textContent = guess_grid_json[0][4];

            guess_grid.children[5].dataset.letter = guess_grid_json[0][5];
            guess_grid.children[5].textContent = guess_grid_json[0][5];
        }
        if (length >= 2) {
            guess_grid.children[6].dataset.letter = guess_grid_json[1][0];
            guess_grid.children[6].textContent = guess_grid_json[1][0];

            guess_grid.children[7].dataset.letter = guess_grid_json[1][1];
            guess_grid.children[7].textContent = guess_grid_json[1][1];

            guess_grid.children[8].dataset.letter = guess_grid_json[1][2];
            guess_grid.children[8].textContent = guess_grid_json[1][2];

            guess_grid.children[9].dataset.letter = guess_grid_json[1][3];
            guess_grid.children[9].textContent = guess_grid_json[1][3];

            guess_grid.children[10].dataset.letter = guess_grid_json[1][4];
            guess_grid.children[10].textContent = guess_grid_json[1][4];

            guess_grid.children[11].dataset.letter = guess_grid_json[1][5];
            guess_grid.children[11].textContent = guess_grid_json[1][5];

        }

        if (length >= 3) {
            guess_grid.children[12].dataset.letter = guess_grid_json[2][0];
            guess_grid.children[12].textContent = guess_grid_json[2][0];

            guess_grid.children[13].dataset.letter = guess_grid_json[2][1];
            guess_grid.children[13].textContent = guess_grid_json[2][1];

            guess_grid.children[14].dataset.letter = guess_grid_json[2][2];
            guess_grid.children[14].textContent = guess_grid_json[2][2];

            guess_grid.children[15].dataset.letter = guess_grid_json[2][3];
            guess_grid.children[15].textContent = guess_grid_json[2][3];

            guess_grid.children[16].dataset.letter = guess_grid_json[2][4];
            guess_grid.children[16].textContent = guess_grid_json[2][4];

            guess_grid.children[17].dataset.letter = guess_grid_json[2][5];
            guess_grid.children[17].textContent = guess_grid_json[2][5];

        }
        if (length >= 4) {
            guess_grid.children[18].dataset.letter = guess_grid_json[3][0];
            guess_grid.children[18].textContent = guess_grid_json[3][0];

            guess_grid.children[19].dataset.letter = guess_grid_json[3][1];
            guess_grid.children[19].textContent = guess_grid_json[3][1];

            guess_grid.children[20].dataset.letter = guess_grid_json[3][2];
            guess_grid.children[20].textContent = guess_grid_json[3][2];

            guess_grid.children[21].dataset.letter = guess_grid_json[3][3];
            guess_grid.children[21].textContent = guess_grid_json[3][3];

            guess_grid.children[22].dataset.letter = guess_grid_json[3][4];
            guess_grid.children[22].textContent = guess_grid_json[3][4];

            guess_grid.children[23].dataset.letter = guess_grid_json[3][5];
            guess_grid.children[23].textContent = guess_grid_json[3][5];
        }
        if (length >= 5) {
            guess_grid.children[24].dataset.letter = guess_grid_json[4][0];
            guess_grid.children[24].textContent = guess_grid_json[4][0];

            guess_grid.children[25].dataset.letter = guess_grid_json[4][1];
            guess_grid.children[25].textContent = guess_grid_json[4][1];

            guess_grid.children[26].dataset.letter = guess_grid_json[4][2];
            guess_grid.children[26].textContent = guess_grid_json[4][2];

            guess_grid.children[27].dataset.letter = guess_grid_json[4][3];
            guess_grid.children[27].textContent = guess_grid_json[4][3];

            guess_grid.children[28].dataset.letter = guess_grid_json[4][4];
            guess_grid.children[28].textContent = guess_grid_json[4][4];

            guess_grid.children[29].dataset.letter = guess_grid_json[4][5];
            guess_grid.children[29].textContent = guess_grid_json[4][5];
        }
        if (length >= 6) {
            guess_grid.children[30].dataset.letter = guess_grid_json[5][0];
            guess_grid.children[30].textContent = guess_grid_json[5][0];

            guess_grid.children[31].dataset.letter = guess_grid_json[5][1];
            guess_grid.children[31].textContent = guess_grid_json[5][1];

            guess_grid.children[32].dataset.letter = guess_grid_json[5][2];
            guess_grid.children[32].textContent = guess_grid_json[5][2];

            guess_grid.children[33].dataset.letter = guess_grid_json[5][3];
            guess_grid.children[33].textContent = guess_grid_json[5][3];

            guess_grid.children[34].dataset.letter = guess_grid_json[5][4];
            guess_grid.children[34].textContent = guess_grid_json[5][4];

            guess_grid.children[35].dataset.letter = guess_grid_json[5][5];
            guess_grid.children[35].textContent = guess_grid_json[5][5];
        }
        if (length >= 7) {
            guess_grid.children[36].dataset.letter = guess_grid_json[6][0];
            guess_grid.children[36].textContent = guess_grid_json[6][0];

            guess_grid.children[37].dataset.letter = guess_grid_json[6][1];
            guess_grid.children[37].textContent = guess_grid_json[6][1];

            guess_grid.children[38].dataset.letter = guess_grid_json[6][2];
            guess_grid.children[38].textContent = guess_grid_json[6][2];

            guess_grid.children[39].dataset.letter = guess_grid_json[6][3];
            guess_grid.children[39].textContent = guess_grid_json[6][3];

            guess_grid.children[40].dataset.letter = guess_grid_json[6][4];
            guess_grid.children[40].textContent = guess_grid_json[6][4];

            guess_grid.children[41].dataset.letter = guess_grid_json[6][5];
            guess_grid.children[41].textContent = guess_grid_json[6][5];
        }
        if (length >= 8) {
            guess_grid.children[42].dataset.letter = guess_grid_json[7][0];
            guess_grid.children[42].textContent = guess_grid_json[7][0];

            guess_grid.children[43].dataset.letter = guess_grid_json[7][1];
            guess_grid.children[43].textContent = guess_grid_json[7][1];

            guess_grid.children[44].dataset.letter = guess_grid_json[7][2];
            guess_grid.children[44].textContent = guess_grid_json[7][2];

            guess_grid.children[45].dataset.letter = guess_grid_json[7][3];
            guess_grid.children[45].textContent = guess_grid_json[7][3];

            guess_grid.children[46].dataset.letter = guess_grid_json[7][4];
            guess_grid.children[46].textContent = guess_grid_json[7][4];

            guess_grid.children[47].dataset.letter = guess_grid_json[7][5];
            guess_grid.children[47].textContent = guess_grid_json[7][5];
        }


    }
    played = json_file.played_count;
    win_per = json_file.percent;
    cur_strk = json_file.streak;
    max_strk = json_file.max_streak;
    guess_1 = json_file.guess_1;
    guess_2 = json_file.guess_2;
    guess_3 = json_file.guess_3;
    guess_4 = json_file.guess_4;
    guess_5 = json_file.guess_5;
    guess_6 = json_file.guess_6;
    guess_7 = json_file.guess_7;
    guess_8 = json_file.guess_8;
    if (Math.floor(dayOffset) > json_file.day+1) {
        if (cur_strk > max_strk) {
            max_strk = cur_strk;
        }
        cur_strk = 0;
        exportData();
    }
    else if (Math.floor(dayOffset) > json_file.day) {
        if (cur_strk > max_strk) {
            win = false;
            lose = false;
            has_played = false;
            guess_grid_json = [
                [],
                [],
                [],
                [],

                [],
                [],
                [],
                [],
            ];

        }
        exportData();
    }
}

function exportData () {
    json_file.played_count = played;
    json_file.percent = win_per;
    json_file.streak = cur_strk;
    json_file.max_streak = max_strk;
    json_file.guess_1 = guess_1;
    json_file.guess_2 = guess_2;
    json_file.guess_3 = guess_3;
    json_file.guess_4 = guess_4;
    json_file.guess_5 = guess_5;
    json_file.guess_6 = guess_6;
    json_file.guess_7 = guess_7;
    json_file.guess_8 = guess_8;
    json_file.day = Math.floor(dayOffset);
    json_file.played_letter = guess_grid_json;
    json_file.win = win;
    json_file.lose = lose;
    json_file.played = has_played;
    console.log(json_file);
    fs.writeFile(`${__dirname}/data.json`, JSON.stringify(json_file, null, "\t"), (err) => {
        if (err) console.log(err);
    });
}



onmousedown = function(e){
    x = e.clientX;
    y = e.clientY;
}
function showStats()
{   
    s2.style.display = "block";
    stats_screen.style.display = "block";
}






function calculateWinPercentage () {
    var total = played;
    var correct = guess_1 + guess_2 + guess_3 + guess_4 + guess_5 + guess_6 + guess_7 + guess_8;
    if (total == 0) {
        return 0;
    }
    var win_per = Math.round((correct/total)*100);
    return win_per;
}

function updateStats () {
    played_div.textContent = played;
    per_div.textContent = win_per+"%";
    ms_div.textContent = max_strk;
    cs_div.textContent = cur_strk;
    if (win || lose) {
        document.getElementById("foot").style.display = "";
    }
    else {
        document.getElementById("foot").style.display = "none";
    }
    distrib_div.children[0].textContent = '1: '+guess_1;
    distrib_div.children[1].textContent = '2: '+guess_2;
    distrib_div.children[2].textContent = '3: '+guess_3;
    distrib_div.children[3].textContent = '4: '+guess_4;
    distrib_div.children[4].textContent = '5: '+guess_5;
    distrib_div.children[5].textContent = '6: '+guess_6;
    distrib_div.children[6].textContent = '7: '+guess_7;
    distrib_div.children[7].textContent = '8: '+guess_8;
    if (win) {
    distrib_div.children[guess_count-2].style.color = "rgb(0,230,0)";
    }

}


function hideStats() {
    if ((x <= 494 || x >= 944)) {
        s2.classList.add("hide");
        s2.addEventListener("animationend", () => {
            setTimeout(() => {
                stats_screen.style.display = "none";
            }, 200);
            s2.style.display = "none";
            s2.classList.remove("hide");
        }, {once: true});
    }
     if (y <= 190 || y >= 690) {
        s2.classList.add("hide");
        s2.addEventListener("animationend", () => {
            setTimeout(() => {
                stats_screen.style.display = "none";
            }, 200);
            s2.style.display = "none";
            s2.classList.remove("hide");

        }, {once: true});
    }
    x = 0;
    y = 0;
}


function createOutput() {
    var output = "Sesordle ";
    output += Math.floor(dayOffset) + " ";
    if (win) {
    output += (guess_count-1)+"/8\n";
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
        if (counter == 6 && guess_grid.children[i+1].dataset.state != undefined) {
            output += "\n";
            counter = 1;
        }
        else {
        counter ++;
        }
    }    

    return output;
}

function copyToClipboard () {
    clipboard.writeText(createOutput());
    showAlert("Copied to clipboard!");
}

function startInteraction () {
    document.addEventListener("keydown", handleKeyPress);
  }
  
function pressKey(key) {
    const activeTiles = getActiveTiles();
    if (activeTiles.length >= 6) return
    const next = guess_grid.querySelector(":not([data-letter])")
    next.dataset.letter = key.toLowerCase()
    next.textContent = key
    next.dataset.state = "active"
}
  
  function handleKeyPress(e) {
    if (e.key === "Enter") {
      submitGuess();
      return;
    }
    if (e.key === "Backspace" || e.key === "Delete") {
      deleteKey();
      return;
    }
    if (e.key.toLowerCase().match(/^[a-z]$/)) {
      pressKey(e.key);
      return;
    }
  
}

  function getActiveTiles() {
    return guess_grid.querySelectorAll("[data-state='active']")
}
  
  function stopInteraction () {
    document.removeEventListener("keydown", handleKeyPress);
}

function deleteKey () {
    const activeTiles = getActiveTiles();
    const lastTile = activeTiles[activeTiles.length - 1];
    if (lastTile == null) return;
    lastTile.textContent = "";
    delete lastTile.dataset.letter;
    delete lastTile.dataset.state;
}

function submitGuess () {
    const activeTiles = [...getActiveTiles()]
    if (activeTiles.length < 6) {
     if (alertContainer.children.length <3){
        showAlert("Not enough letters")
     shakeTiles(activeTiles)
     }
    return
    }
    const guess = activeTiles.reduce((word, tile) => {
        return word + tile.dataset.letter
    }, "")
    stopInteraction()
    if (!has_played) {
        has_played = true;
    }
        
    letterCount = {};
    for (let i = 0; i < guess.length; i++) {
        if (letterCount[targetWord[i]]) {
            letterCount[targetWord[i]] += 1;
        }
        else {
            letterCount[targetWord[i]] = 1;
        }
    }
    activeTiles.forEach((...params) => flipTiles(...params, guess, letterCount))
    guess_count++;
    exportData();

}
    





function showAlert(message, duration = 1000) {
    const alert = document.createElement("div");
    alert.textContent = message;
    alert.classList.add("alert");
    alertContainer.prepend(alert);
    if (duration == null) return 
    setTimeout(() => {
        alert.classList.add("hide")
        alert.addEventListener("transitionend", () => {
            alert.remove()
        });
    }, duration);
    
} 

function shakeTiles(tiles) {
    tiles.forEach(tile => {
        tile.classList.add("shake")
        tile.addEventListener("animationend", () => {
            tile.classList.remove("shake")
        }, {once: true});
    });
}

function flipTiles (tile, index, array, guess, lc) {
    const letter = tile.dataset.letter;
    setTimeout(() => {
        tile.classList.add("flip")
    }, (index*500)/2);




    tile.addEventListener("transitionend", () => {
        tile.classList.remove("flip")
        /// check if all are correct first to fix bug
        if (targetWord[index] === letter) {
            tile.dataset.state = "correct"
            lc[letter] -= 1;
        } 

        if (tile.dataset.state !== "correct") {
        if (targetWord.includes(letter) && lc[letter] > 0) {
            tile.dataset.state = "wrong-location"
            lc[letter] -= 1;
        } else  {
            tile.dataset.state = "wrong"
        }
    }

        if (index === array.length - 1) {
            tile.addEventListener("transitionend", () => {
                startInteraction()
                checkWinLose(guess,array);
            }, {once: true});
        }
    },  {once: true});
    guess_grid_json[guess_count-1][index] = letter;



}

function checkWinLose(guess, tiles) {
    if (guess == targetWord) {
        switch (guess_count) {
            case 2:
                showAlert("Genius!",1000)
                guess_1++;
                break;
            case 3:
                showAlert("Master!",1000)
                guess_2++;
                break;
            case 4:
                showAlert("Expert!",1000)
                guess_3++;
                break;
            case 5:
                showAlert("Pro!",1000)
                guess_4++;
                break;
            case 6:
                showAlert("Whew!",1000)
                guess_5++;
                break;
            case 7:
                showAlert("Phew!",1000)
                guess_6++;
                break;
            case 8:
                showAlert("So Close!",1000)
                guess_7++;
                break;
            case 9:
                showAlert("Too Close!",1000)
                guess_8++;
                break;
        }
        win = true;
        played++
        cur_strk++;
        win_per = calculateWinPercentage();
        if (cur_strk > max_strk) {
            max_strk = cur_strk;
        }
        updateStats();
    } else if (guess_count == 9) {
            showAlert(targetWord.toUpperCase(),1000)
            win = false;
            lose = true;
            stopInteraction()
            setTimeout(() => {
                showStats();
            return
        }, 1700);
        played++;
        cur_strk = 0;
        win_per = calculateWinPercentage();
        if (cur_strk > max_strk) {
            max_strk = cur_strk;
        }
        updateStats();
        return;
     }
    if (win) {
    
        danceTiles(tiles)
        stopInteraction()
        setTimeout(() => {
            showStats();
        return
    }, 1400);
}
}


function danceTiles(tiles) {
    tiles.forEach((tile, index) => {
        setTimeout(() => {
        tile.classList.add("dance")
        tile.addEventListener("animationend", () => {
            tile.classList.remove("dance")
        }, {once: true});
        }, (index*500)/5);
    });
}


startInteraction();
importData();

updateStats();
