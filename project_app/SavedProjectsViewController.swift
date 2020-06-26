//
//  SavedProjectsViewController.swift
//  project_app
//
//  Created by Никита Борисов on 12.06.2020.
//  Copyright © 2020 Arina Goloubitskaya. All rights reserved.
//

import Foundation
import UIKit


class SavedProjectsViewController: UIViewController, UITableViewDelegate {

    
    
    var pURL: URL!
    
    var fileNames: [String]!
    
    @IBOutlet weak var tableView: UITableView!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        do {
            let documentURL = FileManager.default.urls(for: .documentDirectory, in: .userDomainMask).first!
            let directoryContents = try! FileManager.default.contentsOfDirectory(at: documentURL, includingPropertiesForKeys: nil, options: [])
            fileNames = directoryContents.map{ $0.deletingPathExtension().lastPathComponent }

        }  catch {
                  print(error.localizedDescription)
        }
        
        self.tableView.register(UITableViewCell.self, forCellReuseIdentifier: "Cell")

        
        tableView.delegate = self
        tableView.dataSource = self
    
    }
    

    
    @IBAction func goButton(_ sender: Any) {
        acceptProject(projectURL: pURL)
    }
    
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        if segue.identifier == "chooseProject" {
            let dvc = segue.destination as! SavedProjectsPickerViewController
            dvc.photoURL  = pURL
            print(pURL)
        }
    }
    
    func acceptProject(projectURL: URL) {
        let photoURL = projectURL.appendingPathComponent("png")
        let grURL = projectURL.appendingPathComponent("groups")
        let kpURL = projectURL.appendingPathComponent("kp")
                    
        //вызываем функцию сохпранения всяких штук проекта
        MatchingAlgorithmsBridge().readGroups(grURL)
        MatchingAlgorithmsBridge().readKeypoints(kpURL)
        MatchingAlgorithmsBridge().confirmProject(0)
        
        pURL = photoURL
        
        performSegue(withIdentifier: "chooseProject", sender: self)
        
    }
    
    func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
       // print("you tapped me")
        let documentURL = FileManager.default.urls(for: .documentDirectory, in: .userDomainMask).first!
        let prURL = documentURL.appendingPathComponent(fileNames[indexPath.row])
        acceptProject(projectURL: prURL)
        
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
}


extension SavedProjectsViewController: UITableViewDataSource {
    func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return fileNames.count
    }
    
    func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        let cell = self.tableView.dequeueReusableCell(withIdentifier: "Cell", for: indexPath)
        cell.textLabel?.text = fileNames[indexPath.row]
        return cell
    }
    
    // this method handles row deletion
    func tableView(_ tableView: UITableView, commit editingStyle: UITableViewCell.EditingStyle, forRowAt indexPath: IndexPath) {

         if editingStyle == .delete {
            
            let documentURL = FileManager.default.urls(for: .documentDirectory, in: .userDomainMask).first!
            let prURL = documentURL.appendingPathComponent(fileNames[indexPath.row])
            
            try! FileManager.default.removeItem(at: prURL)

            // remove the item from the data model
            fileNames.remove(at: indexPath.row)

            
            // delete the table view row
            tableView.deleteRows(at: [indexPath], with: .fade)

         } else if editingStyle == .insert {
             // Not used in our example, but if you were adding a new row, this is where you would do it.
         }
     }
    
    
}
