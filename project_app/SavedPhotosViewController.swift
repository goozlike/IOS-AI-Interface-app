//
//  SavedPhotosViewController.swift
//  project_app
//
//  Created by Arina Goloubitskaya on 15.03.20.
//  Copyright © 2020 Arina Goloubitskaya. All rights reserved.
//

import UIKit

class SavedPhotosViewController: UIViewController, UITextFieldDelegate {

    
    //возвращаемся обратно в меню
    @IBAction func BackToMenuButton(_ sender: Any) {
        self.dismiss(animated: true, completion: nil)
    }
    
    //переход к камере
    @IBAction func GoToOrb(_ sender: Any) {
        performSegue(withIdentifier: "GoToOrb", sender: self)
    }
    
    
    //функция чтобы при нажатии ввод клава убиралась
    func textFieldShouldReturn(_ textField: UITextField) -> Bool {
        self.view.endEditing(true)
        return false
    }
    
    //переменная для передачи картинки из первого окна
    var newImage: UIImage!
    
    //окно картинки с кейпоинтсами
    @IBOutlet weak var ImageWithKeypoints: UIImageView!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        ImageWithKeypoints.image = newImage
        
        //вызываем функцию поиска пездатых кейпоинтсов и получаем массив из них
        //проходим по нему и создаем кнопки с нужными координатами
        
        //создаем кнопку с координатами
        let button = UIButton(frame: CGRect(x: 100, y: 100, width: 100, height: 30))
        button.backgroundColor = .green
        button.setTitle("keypoint", for: .normal)
        button.addTarget(self, action: #selector(buttonAction), for: .touchUpInside)
        

        self.view.addSubview(button);

        // Do any additional setup after loading the view.
    }
    
    
    //действие для кнопки
    @objc func buttonAction(sender: UIButton!) {
        
        
        //создаем текстововое окно на месте этой кнопки с таким же размером
        let label = UITextField(frame: sender.frame)
        label.backgroundColor = .red
        label.delegate = self
        self.view.addSubview(label)
        
        //стираем кнопку
        sender.removeFromSuperview()
        
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    

    /*
    // MARK: - Navigation

    // In a storyboard-based application, you will often want to do a little preparation before navigation
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        // Get the new view controller using segue.destinationViewController.
        // Pass the selected object to the new view controller.
    }
    */

}
