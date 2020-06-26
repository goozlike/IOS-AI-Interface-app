//
//  PopUpViewController.swift
//  project_app
//
//  Created by Никита Борисов on 16.06.2020.
//  Copyright © 2020 Arina Goloubitskaya. All rights reserved.
//

import Foundation
import UIKit

class PopUpViewController: UIViewController {
    
    var text: String!
    
    @IBOutlet weak var label: UILabel!
    
    @IBOutlet weak var messageView: UIView!
    
    @IBAction func closePopUp(_ sender: Any) {
        self.view.removeFromSuperview()
    }
    override func viewDidLoad() {
        super.viewDidLoad()
        messageView.layer.cornerRadius = 24
        
        if text != nil {
            label.text = text;
        }
        // Do any additional setup after loading the view.
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
}
