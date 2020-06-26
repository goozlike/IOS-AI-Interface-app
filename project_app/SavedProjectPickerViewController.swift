//
//  SavedProjectPickerViewController.swift
//  project_app
//
//  Created by Никита Борисов on 17.06.2020.
//  Copyright © 2020 Arina Goloubitskaya. All rights reserved.
//

import Foundation

import UIKit


class SavedProjectsPickerViewController: UIViewController, UITextFieldDelegate {
    
    var photoURL: URL!
    var num: Int!
    var text_f: String!
    
    
    //возвращаемся обратно в меню
    @IBAction func backButton(_ sender: Any) {
        self.dismiss(animated: true, completion: nil)
    }
    
    
    @IBOutlet weak var ImageView: UIImageView!
    
    
    
    //функция чтобы при нажатии ввод клава убиралась
    func textFieldShouldReturn(_ textField: UITextField) -> Bool {
        self.view.endEditing(true)
        text_f = textField.text
        return false
    }

    //переходим к видео
    @IBAction func readyButton(_ sender: Any) {
        if text_f != nil {
            performSegue(withIdentifier: "GoToORB", sender: self)
            
        } else {
            
            let popUpVC = UIStoryboard(name: "Main", bundle: nil).instantiateViewController(withIdentifier: "popUpVCid") as! PopUpViewController
            
            popUpVC.text = "Выберите точку и введите текст"
            
            self.addChild(popUpVC)
            popUpVC.view.frame = self.view.frame
            self.view.addSubview(popUpVC.view)
                
            popUpVC.didMove(toParent: self)
        }
    }
    
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        if segue.identifier == "GoToORB" {
            let dvc = segue.destination as! VideoViewController
                
            dvc.id  = num
            dvc.text = text_f
            dvc.newImage = ImageView.image
        }
    }
    
    override func viewDidLoad() {
        print("here")
        super.viewDidLoad()
        
        let photoData = try? Data(contentsOf: photoURL!)
        
        guard
            let landscapeImage = UIImage(data: photoData!),
            let landscapeCGImage = landscapeImage.cgImage
        else { return }
        let portraitImage = UIImage(cgImage: landscapeCGImage, scale: landscapeImage.scale, orientation: .right)
        
        ImageView.image = portraitImage;
        
        MatchingAlgorithmsBridge().applPhoto(portraitImage)
        
        //вызываем функцию поиска пездатых кейпоинтсов и получаем массив из них
        let pointer = MatchingAlgorithmsBridge().getCoords(0)

            
        //делаем массив
        var array = Array(UnsafeBufferPointer(start: pointer, count: 20))
            
        for i in 0...9 {
            array[2 * i + 1] = Float((3024 - CGFloat(array[2 * i + 1])) * 414 / 3024)
            array[2 * i] = Float(ImageView.frame.origin.y + (CGFloat(array[2 * i])) * 552 / 4032)
            print(array[2 * i + 1], array[2 * i])
                
        }
            
            //проходим по нему и создаем кнопки с нужными координатами
        for i in 0...9 {
            //создаем кнопку с координатами
            makeButton(id: i, x: CGFloat(array[2 * i + 1]), y: CGFloat(array[2 * i]))
        }
    }


            // Do any additional setup after loading the view.

        
        //функция для создания кнопок
        
    func makeButton(id: Int, x: CGFloat, y: CGFloat) {
        let button = UIButton()
        button.frame = CGRect(x: x, y: y, width: 20, height: 20)
           
        button.backgroundColor = .green
        button.setTitle("\(id)", for: .normal)
        button.addTarget(self, action: #selector(buttonAction), for: .touchUpInside)
        button.tag = id
        self.view.addSubview(button);
            
    }
        
        //действие для кнопки
    @objc func buttonAction(sender: UIButton!) {
            
        //создаем текстововое окно на месте этой кнопки с таким же размером
        let label = UITextField(frame: sender.frame)
        label.backgroundColor = .red
        label.delegate = self
        self.view.addSubview(label)
        num = sender.tag
            
            
        //стираем кнопку
        sender.removeFromSuperview()
            
    }



    
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
}
