const { app, BrowserWindow, ipcMain, screen } = require('electron');
const path = require('path');

// picture code
// const {globalShortcut, desktopCapturer} = require('electron');
// const fs = require('fs');
// end picture code



const windowLocationTarget = app.isPackaged ? 'windowLocationProd' : 'windowLocationDev';
const windowSizeTarget = app.isPackaged ? 'windowSizeProd' : 'windowSizeDev';

// Handle creating/removing shortcuts on Windows when installing/uninstalling.
if (require('electron-squirrel-startup')) {
  // eslint-disable-line global-require
  app.quit();
}




const initializeSesordle = () => {
  // Create the browser window.
  const mainWindow = new BrowserWindow({
    width: 830,
    height: 740,
    minWidth: 830,
    minHeight: 740,
    show: false,
    fullscreenable:false,
    maximizable:false,
    titleBarStyle: 'hiddenInset',
    titleBarOverlay: true,
    webPreferences: {
      nodeIntegration: true,
      contextIsolation: false,
      devTools: !app.isPackaged
    }
  });
  const display = screen.getPrimaryDisplay();
  const { x, y } = display.bounds;

  mainWindow.once('ready-to-show', () => {
    
    BrowserWindow.getAllWindows()[0].webContents.send('getType','returnWindowSize',windowSizeTarget);
    BrowserWindow.getAllWindows()[0].webContents.send('getType','returnWindowLocation',windowLocationTarget);

    ipcMain.on('returnWindowSize', (event, arg) => {
      if (mainWindow === null) {
        return;
      }
      if (arg === null) {
        mainWindow.maximize();
        const location = {x : mainWindow.getPosition()[0], y : mainWindow.getPosition()[1]}
        const size = {width : mainWindow.getSize()[0], height : mainWindow.getSize()[1]}
        BrowserWindow.getAllWindows()[0].webContents.send('saveType',windowLocationTarget,location);
        BrowserWindow.getAllWindows()[0].webContents.send('saveType',windowSizeTarget,size);
      }else {
        mainWindow.setBounds({
          width: arg.width,
          height: arg.height
        })
      }
    });

    ipcMain.on('returnWindowLocation', (event, arg) => {
      if (mainWindow === null) {
        return;
      }
      console.log(JSON.stringify(arg))
      if (arg === null) {
        mainWindow.setPosition(x, y+25);
      }else {
        mainWindow.setPosition(arg.x, arg.y);
      }
    });

    ipcMain.on("toggleMaximize", () => {
      if (mainWindow === null) {
        return;
      }
      if (mainWindow.isMaximized()) {
        mainWindow.unmaximize();
      } else {
        mainWindow.maximize();
        const location = {x : mainWindow.getPosition()[0], y : mainWindow.getPosition()[1]}
        const size = {width : mainWindow.getSize()[0], height : mainWindow.getSize()[1]}
        BrowserWindow.getAllWindows()[0].webContents.send('saveType',windowLocationTarget,location);
        BrowserWindow.getAllWindows()[0].webContents.send('saveType',windowSizeTarget,size);
      }
    });
    setTimeout(() => {
    mainWindow.show();
    }, 500);

  })

  mainWindow.on('resize', () => {
    // get the x and y coordinates of the window
    if (mainWindow === null || !mainWindow.isVisible()) {
      return;
    }
    const size = {width : mainWindow.getSize()[0], height : mainWindow.getSize()[1]}
    BrowserWindow.getAllWindows()[0].webContents.send('saveType',windowSizeTarget, size);
  })
  mainWindow.on('move', () => {
    if (mainWindow === null || !mainWindow.isVisible()) {
      return;
    }
    const position = mainWindow.getPosition();
    const positions = {x : position[0], y : position[1]}
    BrowserWindow.getAllWindows()[0].webContents.send('saveType',windowLocationTarget,positions);
  });


  //picture code
  // globalShortcut.register('Command+S', async () => {
  //     nativeImage = await BrowserWindow.getAllWindows()[0].webContents.capturePage().then(image => {
  //           fs.writeFileSync(app.getAppPath()+'screenshot'+(Math.floor(Math.random() * 100) + 1)+'.png', image.toPNG(), (err) => {
  //               if (err) throw err
  //           })
  //           console.log('It\'s saved!')
  //           return image.toDataURL()

  //       })
  // })
  // end picture code

  app.once('before-quit', () => {
    // save the window size and position
    if (mainWindow === null) {
      return;
    }
    // comment these next 4 lines out to set window size and position both to null in localstorage. ONLY FOR BUILD
    const location = {x : mainWindow.getPosition()[0], y : mainWindow.getPosition()[1]}
    const size = {width : mainWindow.getSize()[0], height : mainWindow.getSize()[1]}
    BrowserWindow.getAllWindows()[0].webContents.send('saveType',windowLocationTarget,location);
    BrowserWindow.getAllWindows()[0].webContents.send('saveType',windowSizeTarget,size);

  });


  // and load the index.html of the app.
  mainWindow.loadFile(path.join(__dirname, 'index.html'));

};

// This method will be called when Electron has finished
// initialization and is ready to create browser windows.
// Some APIs can only be used after this event occurs.
app.on('ready', initializeSesordle);






// Quit when all windows are closed, except on macOS. There, it's common
// for applications and their menu bar to stay active until the user quits
// explicitly with Cmd + Q.
app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') {
    app.quit();
  }
});



// picture code
// app.on('will-quit', () => {
//   // Unregister a shortcut.
//   globalShortcut.unregisterAll();
// });
// end picture code



app.on('activate', () => {
  // On OS X it's common to re-create a window in the app when the
  // dock icon is clicked and there are no other windows open.
  if (BrowserWindow.getAllWindows().length === 0) {
    initializeSesordle();
  }
});