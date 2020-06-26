//
//  ViewController.swift
//  project_app
//
//  Created by Arina Goloubitskaya on 14.03.20.
//  Copyright © 2020 Arina Goloubitskaya. All rights reserved.
//

import UIKit
import AVFoundation



class ViewController: UIViewController, UINavigationControllerDelegate, UIImagePickerControllerDelegate {
    @IBOutlet weak var imageView: UIImageView!
    var imagePickerController : UIImagePickerController!

    //переходим на экран с транслирующейся камерой
    @IBAction func goTocamera(_ sender: Any) {
        performSegue(withIdentifier: "VideoSegue", sender: self)
    }
    
    //кнопка для перехода на экран с сохраннеными штуками
    @IBAction func SavedProjectButton(_ sender: UIButton) {
        performSegue(withIdentifier: "SavedProjectsSegue", sender: self)
        
    }
    
    
    //конпка  которая фоткает
    @IBAction func TakePhotoButton(_ sender: UIButton) {
        imagePickerController = UIImagePickerController()
        imagePickerController.delegate = self
        imagePickerController.sourceType = .camera
        present(imagePickerController, animated: true, completion: nil)
        
    }
    
    //переходим на экран с кейпоинтсами
    @IBAction func SavedPhotos(_ sender: UIButton) {
        
        if imageView.image != nil {
            performSegue(withIdentifier: "gallerySegue", sender: self)
        } else {
            
            let popUpVC = UIStoryboard(name: "Main", bundle: nil).instantiateViewController(withIdentifier: "popUpVCid") as! PopUpViewController
            
            self.addChild(popUpVC)
            popUpVC.view.frame = self.view.frame
            self.view.addSubview(popUpVC.view)
            
            popUpVC.didMove(toParent: self)
            
        }
        
    }
    
    //передаем картинку на другой вью контролер
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        if segue.identifier == "gallerySegue" {
            let dvc = segue.destination as! SavedPhotosViewController
            dvc.newImage = imageView.image
        }
    }
    
    //загружаем из галереи
    @IBAction func UploadFromGalleryBotton(_ sender: UIButton) {
        
        
        let image = UIImagePickerController()
        image.delegate = self
        
        image.sourceType = UIImagePickerController.SourceType.photoLibrary
        
        image.allowsEditing = false
        
        self.present(image, animated: true)
        {
            //fljnvksnd
        }
    }
    
    func imagePickerController(_ picker: UIImagePickerController, didFinishPickingMediaWithInfo info: [UIImagePickerController.InfoKey : Any]) {
// Local variable inserted by Swift 4.2 migrator.
let info = convertFromUIImagePickerControllerInfoKeyDictionary(info)

    
        let image = info[convertFromUIImagePickerControllerInfoKey(UIImagePickerController.InfoKey.originalImage)] as? UIImage
        imageView.image = image
        picker.dismiss(animated: true, completion: nil)
    }
    
    func getImage(imageName: String){
        let fileManager = FileManager.default
        let imagePath = (NSSearchPathForDirectoriesInDomains(.documentDirectory, .userDomainMask, true)[0] as NSString).appendingPathComponent(imageName)
        if fileManager.fileExists(atPath: imagePath){
            imageView.image = UIImage(contentsOfFile: imagePath)
        }else{
            print("Panic! No Image!")
        }
    }
    
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view, typically from a nib.
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        
        // Dispose of any resources that can be recreated.
    }


}

// Helper function inserted by Swift 4.2 migrator.
fileprivate func convertFromUIImagePickerControllerInfoKeyDictionary(_ input: [UIImagePickerController.InfoKey: Any]) -> [String: Any] {
	return Dictionary(uniqueKeysWithValues: input.map {key, value in (key.rawValue, value)})
}

// Helper function inserted by Swift 4.2 migrator.
fileprivate func convertFromUIImagePickerControllerInfoKey(_ input: UIImagePickerController.InfoKey) -> String {
	return input.rawValue
}
