//
//  RoundButton.swift
//  project_app
//
//  Created by Arina Goloubitskaya on 15.03.20.
//  Copyright © 2020 Arina Goloubitskaya. All rights reserved.
//

import UIKit

@IBDesignable
class RoundButton: UIButton, UIImagePickerControllerDelegate {
    @IBInspectable var cornerRadius: CGFloat = 0{
        didSet{
            self.layer.cornerRadius = cornerRadius
        }
    }
    
    @IBInspectable var borderWidth: CGFloat = 0{
        didSet{
            self.layer.borderWidth = borderWidth
        }
    }
    
    @IBInspectable var borderColor: UIColor = UIColor.clear{
        didSet{
            self.layer.borderColor = borderColor.cgColor
        }
    }
}
