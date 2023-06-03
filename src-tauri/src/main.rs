#![cfg_attr(not(debug_assertions), windows_subsystem = "windows")]


use tauri::{CustomMenuItem, Menu, MenuItem, Submenu};
use open;




fn main() {
    let quit = CustomMenuItem::new("report_bug".to_string(), "Report a Bug").accelerator("CmdOrCtrl+Shift+S");
    let close = CustomMenuItem::new("itch_page".to_string(), "Itch.io page").accelerator("CmdOrCtrl+Shift+I");
    let submenu = Submenu::new("Help", Menu::new().add_item(quit).add_item(close));
    let menu = Menu::os_default("Sesordle")
  .add_native_item(MenuItem::Copy)
  .add_item(CustomMenuItem::new("hide", "Hide"))
  .add_submenu(submenu);
    tauri::Builder::default()
        .menu(menu)
        .on_menu_event(|event| {
            println!("{:?}", event.menu_item_id());
            match event.menu_item_id() {
                "report_bug" => {
                    let url = "https://github.com/Elon-Musk-YAY/Sesordle-JS/issues/new";
                    open_website(url.to_string());
                }
                "itch_page" => {
                    let url = "https://elon-musk-yay.itch.io/sesordle";
                    open_website(url.to_string());
                }
                _ => {}
            }
        })
        .invoke_handler(tauri::generate_handler![])
        .run(tauri::generate_context!())
        .expect("error while running tauri application");
}

fn open_website(url: String) {
    if let Err(e) = open::that(url) {
        // show a native error dialog
        eprintln!("{}", e);
    }
}