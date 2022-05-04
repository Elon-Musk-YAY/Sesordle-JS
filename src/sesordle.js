

const alertContainer = document.querySelector("[data-alert-container]")
const guess_grid = document.querySelector("[data-guess-grid]")
const targetWords = ["tromie", "boxers"]
var guess_count = 1;
var win = false;
var lose = false;
const {clipboard} = require('electron');

const offsetFromDate = new Date(2022,4,3)
const msOffset = Date.now() - offsetFromDate
const dayOffset = msOffset / 1000 / 60 / 60 / 24
const targetWord = targetWords[Math.floor(dayOffset)]
const stats_screen = document.getElementById("stats_screen");
const s2 = document.getElementById("s2");
const played_div = document.querySelector("#stat-container-played #stat");
const per_div = document.querySelector("#stat-container-win-per #stat");
const ms_div = document.querySelector("#stat-container-max-streak #stat");
const cs_div = document.querySelector("#stat-container-cur-streak #stat");
document.getElementById("stats-button").onclick = showStats;
stats_screen.addEventListener("click", hideStats);
var x, y;

var played, win_per, cur_strk, max_strk, guess_1, guess_2, guess_3, guess_4, guess_5, guess_6, guess_7, guess_8;
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

played_div.textContent = played;
per_div.textContent = win_per+"%";
ms_div.textContent = max_strk;
cs_div.textContent = cur_strk;
onmousedown = function(e){
    x = e.clientX;
    y = e.clientY;
}
function showStats()
{   
    s2.style.display = "block";
    stats_screen.style.display = "block";
    console.log(createOutput())
}

function importData () {

}

function exportData (data) {
}


function calculateWinPercentage () {
    var total = played;
    var correct = guess_1 + guess_2 + guess_3 + guess_4 + guess_5 + guess_6 + guess_7 + guess_8;
    if (total == 0) {
        return 0;
    }
    var win_per = (correct/total)*100;
    return win_per;
}

function updateStats () {
    played_div.textContent = played;
per_div.textContent = win_per+"%";
ms_div.textContent = max_strk;
cs_div.textContent = cur_strk;

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
    // iterate through all tiles in the guess grid
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

function copyToClipboard (text) {
    clipboard.writeText(text);
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
    if (e.key.match(/^[a-z]$/)) {
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
    let letterCount = {};
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
    
}


function createAndCopyOutput  () {

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
        played = 0;
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