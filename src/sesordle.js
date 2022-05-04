const alertContainer = document.querySelector("[data-alert-container]")
const guess_grid = document.querySelector("[data-guess-grid]")
const targetWords = ["tromie", "boxers"]
var guess_count = 1;
var win = false;
const {clipboard} = require('electron');

const offsetFromDate = new Date(2022,4,3)
const msOffset = Date.now() - offsetFromDate
const dayOffset = msOffset / 1000 / 60 / 60 / 24
const targetWord = targetWords[Math.floor(dayOffset)]
const stats_screen = document.getElementById("stats_screen");
const s2 = document.getElementById("s2");

document.getElementById("stats-button").onclick = showStats;
stats_screen.addEventListener("click", hideStats);
var x;
var y;
onmousedown = function(e){
    x = e.clientX;
    y = e.clientY;
}
function showStats()
{
    stats_screen.style.display = "block";
    console.log(createOutput());
}


function hideStats() {
    if ((x <= 494 || x >= 944)) {
        s2.classList.add("hide");
        s2.addEventListener("animationend", () => {
            stats_screen.style.display = "none";
            s2.classList.remove("hide");
        }, {once: true});
    }
    else if (y <= 100 || y >= 600) {
        s2.classList.remove("add");
        s2.addEventListener("animationend", () => {
            stats_screen.style.display = "none";
            s2.classList.remove("hide");

        }, {once: true});
    }
    x = 0;
    y = 0;
}


function createOutput() {
    var output = "Sesordle ";
    output += Math.floor(dayOffset) + " ";
    output += (guess_count-1)+"/8\n";
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
        showAlert("You win!",1000)
        danceTiles(tiles)
        stopInteraction()
        win = true;
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