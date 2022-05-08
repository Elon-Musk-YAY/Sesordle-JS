const _path = require('path');
const fs = require('fs');


/*
* Created by Akshar Desai on 5/6/2022.
*
*/

const alertContainer = document.querySelector("[data-alert-container]")
const guess_grid = document.querySelector("[data-guess-grid]")
const targetWords = ['harley', 'nuptse', 'duplex', 'narrow', 'clinic', 'powder', 'steamy', 'deftly', 'paving', 'bluish', 'wooler', 'elidir', 'rescue', 'manage', 'impede', 'flight', 'ringer', 'wicked', 'feeder', 'coffee', 'abroad', 'brooks', 'hunter', 'damned', 'beetle', 'laying', 'petrel', 'chopin', 'masses', 'dibabs', 'clumsy', 'single', 'tigger', 'depict', 'ogmore', 'female', 'throat', 'scalby', 'dicing', 'enrage', 'alfred', 'colony', 'tirade', 'demand', 'yonder', 'appeal', 'signal', 'cosine', 'handed', 'manuka', 'staple', 'sponge', 'taught', 'curtsy', 'borrow', 'queens', 'tissue', 'basalt', 'tingly', 'minnow', 'tryfan', 'pallid', 'wilson', 'graves', 'forget', 'sudoku', 'babies', 'vulcan', 'decade', 'peeved', 'butter', 'proves', 'thrill', 'reflex', 'wretch', 'unwell', 'launch', 'months', 'pelite', 'liable', 'martha', 'pseudo', 'common', 'breast', 'marton', 'ascend', 'towhee', 'buzzer', 'wellow', 'banked', 'justly', 'ladder', 'yawler', 'shield', 'lithic', 'archan', 'soccer', 'outlet', 'tomato', 'foyers', 'impact', 'shoddy', 'pastie', 'amedei', 'indent', 'exeter', 'brainy', 'pacify', 'bucket', 'skiing', 'cuckoo', 'filter', 'sarnia', 'wrymug', 'loosen', 'winkle', 'amuser', 'connie', 'hannah', 'shabby', 'nature', 'planet', 'thirty', 'anthem', 'trifle', 'belong', 'nitric', 'unison', 'search', 'google', 'cloaks', 'citril', 'sodium', 'rocket', 'giblet', 'upward', 'oxford', 'allude', 'laptop', 'dinner', 'coyote', 'infirm', 'joyous', 'almond', 'barkis', 'fossil', 'island', 'yorkie', 'ankles', 'aflame', 'exhort', 'wilted', 'mumbai', 'obiwan', 'vacuum', 'object', 'presto', 'leader', 'hinney', 'sample', 'gentry', 'pepper', 'ocular', 'shower', 'utmost', 'kodiak', 'slovak', 'bishop', 'drinks', 'carton', 'tromie', 'system', 'across', 'zambia', 'quiver', 'smoked', 'strake', 'uptown', 'haggis', 'client', 'bounce', 'roster', 'durham'];
var guess_count = 1;
var win = false;
var lose = false;
const {clipboard} = require('electron');
const json_file = require(`${__dirname}/data.json`);
let nex_tim = document.getElementById("nex");
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
const s2 = document.getElementById("s2")
const played_div = document.querySelector("#stat-container-played #stat");
const per_div = document.querySelector("#stat-container-win-per #stat");
const ms_div = document.querySelector("#stat-container-max-streak #stat");
const cs_div = document.querySelector("#stat-container-cur-streak #stat");
const distrib_div = document.querySelector("#distribs");
document.getElementById("stats-button").onclick = showStats;
stats_screen.addEventListener("click", hideStats);
s2.addEventListener('click', e => {
    e.stopImmediatePropagation()
})
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
    for (let i = 0; i<8; i++) {
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

function removeByIndex(str,index) {
    return str.slice(0,index) + str.slice(index+1);
}

function calculateResult (index, guess) {
    var temp = targetWord;
    var out = [];
    // check correct letters
    for (let i = 0; i<6; i++) {
        if (targetWord[i] == guess[i]) {
            out[i] = "correct";
            temp = removeByIndex(temp, temp.indexOf(guess[i]));
        }
    }
    for (let i = 0; i<6; i++) {
        if (temp.includes(guess[i]) && out[i] != "correct") {
            out[i] = "wrong-location";
            temp = removeByIndex(temp, temp.indexOf(guess[i]));
        }
        else if (out[i] != "correct")  {
            out[i] = "wrong";
        }

    }

    return out[index];
}

function importData () {
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
    win = json_file.win;
    lose = json_file.lose;
    if (Math.floor(dayOffset) > json_file.day+1) {
        if (cur_strk > max_strk) {
            max_strk = cur_strk;
        }
        cur_strk = 0;
        exportData();
    }
    else if (Math.floor(dayOffset) > json_file.day) {
            win = false;
            lose = false;
            has_played = false;
            guess_grid_json = [
                [],[],[],[],[],[],[],[]
            ];

        exportData();
    }
    if (json_file.played) {
        guess_grid_json = json_file.played_letter;
        var length = getLengthOfJson();
        guess_count = getLengthOfJson()+1;
        if (length >= 1) {
            let g = guess_grid_json[0][0]+guess_grid_json[0][1]+guess_grid_json[0][2]+guess_grid_json[0][3]+guess_grid_json[0][4]+guess_grid_json[0][5];

            for (let i =0; i<6; i++) {
                guess_grid.children[i].dataset.letter = guess_grid_json[0][i];
                guess_grid.children[i].textContent = guess_grid_json[0][i];
                guess_grid.children[i].style.transition = 'transform 100ms linear;';
                guess_grid.children[i].addEventListener("transitionend", () => {
                guess_grid.children[i].classList.remove("fast");
                guess_grid.children[i].dataset.state = calculateResult(i, g);
                guess_grid.children[i].style.transition = 'transform 250ms linear;';
                }, {once: true});
            }
        }
        if (length >= 2) {

            let g = guess_grid_json[1][0]+guess_grid_json[1][1]+guess_grid_json[1][2]+guess_grid_json[1][3]+guess_grid_json[1][4]+guess_grid_json[1][5];

            for (let i =6; i<12; i++) {
                guess_grid.children[i].dataset.letter = guess_grid_json[1][i-6];
                guess_grid.children[i].textContent = guess_grid_json[1][i-6];
                guess_grid.children[i].style.transition = 'transform 100ms linear;';
                guess_grid.children[i].addEventListener("transitionend", () => {
                guess_grid.children[i].classList.remove("fast");
                guess_grid.children[i].dataset.state = calculateResult(i-6, g);
                guess_grid.children[i].style.transition = 'transform 250ms linear;';
                }, {once: true});
            }

        }

        if (length >= 3) {
            let g = guess_grid_json[2][0]+guess_grid_json[2][1]+guess_grid_json[2][2]+guess_grid_json[2][3]+guess_grid_json[2][4]+guess_grid_json[2][5];

            for (let i =12; i<18; i++) {
                guess_grid.children[i].dataset.letter = guess_grid_json[2][i-12];
                guess_grid.children[i].textContent = guess_grid_json[2][i-12];
                guess_grid.children[i].style.transition = 'transform 100ms linear;';
                guess_grid.children[i].addEventListener("transitionend", () => {
                guess_grid.children[i].classList.remove("fast");
                guess_grid.children[i].dataset.state = calculateResult(i-12, g);
                guess_grid.children[i].style.transition = 'transform 250ms linear;';
                }, {once: true});
            }
        }
        if (length >= 4) {
            let g = guess_grid_json[3][0]+guess_grid_json[3][1]+guess_grid_json[3][2]+guess_grid_json[3][3]+guess_grid_json[3][4]+guess_grid_json[3][5];

            for (let i =18; i<24; i++) {
                guess_grid.children[i].dataset.letter = guess_grid_json[3][i-18];
                guess_grid.children[i].textContent = guess_grid_json[3][i-18];
                guess_grid.children[i].style.transition = 'transform 100ms linear;';
                guess_grid.children[i].addEventListener("transitionend", () => {
                guess_grid.children[i].classList.remove("fast");
                guess_grid.children[i].dataset.state = calculateResult(i-18, g);
                guess_grid.children[i].style.transition = 'transform 250ms linear;';
                }, {once: true});
            }
        }
        if (length >= 5) {
            let g = guess_grid_json[4][0]+guess_grid_json[4][1]+guess_grid_json[4][2]+guess_grid_json[4][3]+guess_grid_json[4][4]+guess_grid_json[4][5];

            for (let i =24; i<30; i++) {
                guess_grid.children[i].dataset.letter = guess_grid_json[4][i-24];
                guess_grid.children[i].textContent = guess_grid_json[4][i-24];
                guess_grid.children[i].style.transition = 'transform 100ms linear;';
                guess_grid.children[i].addEventListener("transitionend", () => {
                guess_grid.children[i].classList.remove("fast");
                guess_grid.children[i].dataset.state = calculateResult(i-24, g);
                guess_grid.children[i].style.transition = 'transform 250ms linear;';
                }, {once: true});
            }
        }
        if (length >= 6) {
            let g = guess_grid_json[5][0]+guess_grid_json[5][1]+guess_grid_json[5][2]+guess_grid_json[5][3]+guess_grid_json[5][4]+guess_grid_json[5][5];

            for (let i =30; i<36; i++) {
                guess_grid.children[i].dataset.letter = guess_grid_json[5][i-30];
                guess_grid.children[i].textContent = guess_grid_json[5][i-30];
                guess_grid.children[i].style.transition = 'transform 100ms linear;';
                guess_grid.children[i].addEventListener("transitionend", () => {
                guess_grid.children[i].classList.remove("fast");
                guess_grid.children[i].dataset.state = calculateResult(i-30, g);
                guess_grid.children[i].style.transition = 'transform 250ms linear;';
                }, {once: true});
            }
        }
        if (length >= 7) {
            let g = guess_grid_json[6][0]+guess_grid_json[6][1]+guess_grid_json[6][2]+guess_grid_json[6][3]+guess_grid_json[6][4]+guess_grid_json[6][5];

            for (let i =36; i<42; i++) {
                guess_grid.children[i].dataset.letter = guess_grid_json[6][i-36];
                guess_grid.children[i].textContent = guess_grid_json[6][i-36];
                guess_grid.children[i].style.transition = 'transform 100ms linear;';
                guess_grid.children[i].addEventListener("transitionend", () => {
                guess_grid.children[i].classList.remove("fast");
                guess_grid.children[i].dataset.state = calculateResult(i-36, g);
                guess_grid.children[i].style.transition = 'transform 250ms linear;';
                }, {once: true});
            }
        }
        if (length >= 8) {
            let g = guess_grid_json[7][0]+guess_grid_json[7][1]+guess_grid_json[7][2]+guess_grid_json[7][3]+guess_grid_json[7][4]+guess_grid_json[7][5];

            for (let i =42; i<48; i++) {
                guess_grid.children[i].dataset.letter = guess_grid_json[7][i-42];
                guess_grid.children[i].textContent = guess_grid_json[7][i-42];
                guess_grid.children[i].style.transition = 'transform 100ms linear;';
                guess_grid.children[i].addEventListener("transitionend", () => {
                guess_grid.children[i].classList.remove("fast");
                guess_grid.children[i].dataset.state = calculateResult(i-42, g);
                guess_grid.children[i].style.transition = 'transform 250ms linear;';
                }, {once: true});
            }
        }

        setTimeout(() => {
            for (let i =0; i<length*6; i++) {
                    guess_grid.children[i].classList.add("fast");
            } }, 700);

        }

    

    if (win || lose) {
        setTimeout(
            () => {
                if (!s2.classList.contains("hide")) {
                    showStats();
                }
            }, 2000)
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
    fs.writeFile(`${__dirname}/data.json`, JSON.stringify(json_file, null, "\t"), (err) => {
        if (err) console.log(err);
    });
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
    distrib_div.children[guess_count-2].style.color = "hsl(114, 42%, 59%)";
    }

}


function hideStats() {
        s2.classList.add("hide");
        s2.addEventListener("animationend", () => {
            setTimeout(() => {
                stats_screen.style.display = "none";
            }, 200);
            s2.style.display = "none";
            s2.classList.remove("hide");
        }, {once: true});
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
        if (counter == 6 && guess_grid.children[i+1]) {
            if (guess_grid.children[i+1].dataset.state != undefined) {
            output += "\n";
            counter = 1;
            }
        }
        else {
        counter ++;
        }
    }    

    return output;
}

function copyToClipboard () {
    clipboard.writeText(createOutput());
    showAlert("Copied to clipboard!", 1000, -1);
}

function startInteraction () {
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
        }, 25);
    });
    next.dataset.state = "active"
}


  
  function handleKeyPress(e) {
    if (e.key === "Enter") {
        activeTiles = getActiveTiles();
        for (let i = 0; i < activeTiles.length; i++) {
            const tile = activeTiles[i];
            if (tile.classList.value.includes("expand")) {
                tile.classList.remove("expand");
            }
    }
    setTimeout(() => {
        submitGuess();
    }, 50);
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
    return [guess_grid.children[(guess_count-1)*6],guess_grid.children[(guess_count-1)*6+1],guess_grid.children[(guess_count-1)*6+2],guess_grid.children[(guess_count-1)*6+3],guess_grid.children[(guess_count-1)*6+4],guess_grid.children[(guess_count-1)*6+5]];
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
     shakeTiles([...getGuessTiles()]);
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
    activeTiles.forEach((...params) => flipTiles(...params, guess))
    guess_count++;

}
    





function showAlert(message, duration = 1000, zIndex=0) {
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
        });
    }, duration);
}
    
} 

function shakeTiles(tiles) {
    tiles.forEach(tile => {
        tile.classList.add("shake")
        tile.addEventListener("animationend", () => {
            tile.classList.remove("shake")
        }, {once: true});
    });
}

function flipTiles (tile, index, array, guess) {
    const letter = tile.dataset.letter;
    setTimeout(() => {
        tile.classList.add("flip")
    }, (index*500)/2);

    console.log(index)


    tile.addEventListener("transitionend", () => {
        tile.classList.remove("flip")
        /// check if all are correct first to fix bug
        tile.dataset.state = calculateResult(index, guess);

        

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
            showAlert(targetWord.toUpperCase(),-1)
            win = false;
            lose = true;
            stopInteraction()
            setTimeout(() => {
                if (!s2.classList.contains("hide")) {
                showStats();
                }
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
            if (!s2.classList.contains("hide")) {
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
        }, {once: true});
        }, (index*500)/5);
    });
}


startInteraction();
importData();
if (win || lose) {
    stopInteraction();
}
updateStats();
