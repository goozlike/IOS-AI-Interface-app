//
//  SavedPhotosViewController.swift
//  project_app
//
//  Created by Arina Goloubitskaya on 15.03.20.
//  Copyright © 2020 Arina Goloubitskaya. All rights reserved.
//

import UIKit


class SavedPhotosViewController: UIViewController, UITextFieldDelegate {

    
    var num: Int!
    var text_f: String!
    
    //переменная для передачи картинки из первого окна
    var newImage: UIImage!
    
    //окно картинки с кейпоинтсами
    @IBOutlet weak var ImageWithKeypoints: UIImageView!
    
    //окошко с именем файла
    @IBOutlet weak var FileNameTextField: UITextField!
    
    //функция которая вызывается при нажатии на имЯ файла
    @objc func myTargetFunction(textField: UITextField) {
        FileNameTextField.text = ""
    }
    
    //возвращаемся обратно в меню
    @IBAction func BackToMenuButton(_ sender: Any) {
        self.dismiss(animated: true, completion: nil)
    }
    
    
    //переход к камере
    @IBAction func GoToOrb(_ sender: Any) {
        
        if text_f != nil {
            performSegue(withIdentifier: "GoToOrb", sender: self)
            
        } else {
            let popUpVC = UIStoryboard(name: "Main", bundle: nil).instantiateViewController(withIdentifier: "popUpVCid") as! PopUpViewController
            popUpVC.text = "Выберите точку и введите текст"
            self.addChild(popUpVC)
            popUpVC.view.frame = self.view.frame
            self.view.addSubview(popUpVC.view)
            
            popUpVC.didMove(toParent: self)
        }
    }
    //функция для перехода к камере
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        if segue.identifier == "GoToOrb" {
            let dvc = segue.destination as! VideoViewController
            
            dvc.id  = num
            
            dvc.text = text_f
        }
    }
    
    //функция чтобы при нажатии ввод клава убиралась
    func textFieldShouldReturn(_ textField: UITextField) -> Bool {
        self.view.endEditing(true)
        if textField != FileNameTextField {
            text_f = textField.text
        }
        return false
    }

  


//сохранение проекта
    @IBAction func SaveButton(_ sender: Any) {
        
        //читаем имя проекта
        let fileName = FileNameTextField.text!
        
        //если оно введено и длинна больше 0 то сохраняем иначе ошибка
        if fileName != "Введите имя проекта:" && fileName.count > 0 {
            
            //стоит проверить что такое имя уже не выбрано
            if check(name: fileName) == 0 {
                
                let popUpVC = UIStoryboard(name: "Main", bundle: nil).instantiateViewController(withIdentifier: "popUpVCid") as! PopUpViewController
                
                popUpVC.text = "Такой файл уже существует"
                
                self.addChild(popUpVC)
                popUpVC.view.frame = self.view.frame
                self.view.addSubview(popUpVC.view)
                
                popUpVC.didMove(toParent: self)
                
            } else {
            
                let data = newImage.pngData()!
            
                //делаем путь ~/filename
                let documentsPath = NSURL(fileURLWithPath: NSSearchPathForDirectoriesInDomains(.documentDirectory, .userDomainMask, true)[0])
                let projectURL = documentsPath.appendingPathComponent(fileName)
                let photoURL = projectURL!.appendingPathComponent("png")
                let grURL = projectURL!.appendingPathComponent("groups")
                let kpURL = projectURL!.appendingPathComponent("kp")
            
                //делаем директорию проекта
                do{
                    try FileManager.default.createDirectory(atPath: projectURL!.path, withIntermediateDirectories: true, attributes: nil)
                
                }catch let error as NSError{
                
                    print("Unable to create directory",error)
                }
                        
                //вызываем функцию сохпранения всяких штук проекта
                MatchingAlgorithmsBridge().writeGroups(grURL)
                MatchingAlgorithmsBridge().writeKeypoints(kpURL)

                //сохраняем картинку
            
                do {
                    try data.write(to: photoURL)
                } catch {
                    print("error savind photo")
                }
                
                let popUpVC = UIStoryboard(name: "Main", bundle: nil).instantiateViewController(withIdentifier: "popUpVCid") as! PopUpViewController
                popUpVC.text = "Успешно сохранено"
                self.addChild(popUpVC)
                popUpVC.view.frame = self.view.frame
                self.view.addSubview(popUpVC.view)
                
                popUpVC.didMove(toParent: self)
            }
            
        //eсли длина 0 то не сохраняем
        } else {
            
            let popUpVC = UIStoryboard(name: "Main", bundle: nil).instantiateViewController(withIdentifier: "popUpVCid") as! PopUpViewController
            popUpVC.text = "Введите имя файла"
            self.addChild(popUpVC)
            popUpVC.view.frame = self.view.frame
            self.view.addSubview(popUpVC.view)
            
            popUpVC.didMove(toParent: self)
        }
    }
    
    
    //функция проверки уникальноости файла
    func check(name: String) -> Int
    {
        do {
            let documentURL = FileManager.default.urls(for: .documentDirectory, in: .userDomainMask).first!
            let directoryContents = try FileManager.default.contentsOfDirectory(at: documentURL, includingPropertiesForKeys: nil, options: [])
            
            let FileNames = directoryContents.map{ $0.deletingPathExtension().lastPathComponent }
            
            for fileName in FileNames {
                if name == fileName {
                    return 0
                }
            }
        }
        catch {
            print(error.localizedDescription)
        }
        
        return 1
        
    }
    
    
    
    override func viewDidLoad() {
        super.viewDidLoad()
        ImageWithKeypoints.image = newImage
        
        self.FileNameTextField.delegate = self
        FileNameTextField.addTarget(self, action: #selector(myTargetFunction), for: .touchDown)
        
        //вызываем функцию поиска пездатых кейпоинтсов и получаем массив из них
        let pointer = MatchingAlgorithmsBridge().findBest(newImage)

        
        //делаем массив нормальный
        var array = Array(UnsafeBufferPointer(start: pointer, count: 20))
        
        for i in 0...9 {
            array[2 * i + 1] = Float((3024 - CGFloat(array[2 * i + 1])) * 414 / 3024)
            array[2 * i] = Float(ImageWithKeypoints.frame.origin.y + (CGFloat(array[2 * i])) * 552 / 4032)
            print(array[2 * i + 1], array[2 * i])
            
        }
        
        //проходим по нему и создаем кнопки с нужными координатами
        for i in 0...9 {
            //создаем кнопку с координатами
            makeButton(id: i, x: CGFloat(array[2 * i + 1]), y: CGFloat(array[2 * i]))
        }


        // Do any additional setup after loading the view.
    }
    
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
    
    

    /*
    // MARK: - Navigation

    // In a storyboard-based application, you will often want to do a little preparation before navigation
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        // Get the new view controller using segue.destinationViewController.
        // Pass the selected object to the new view controller.
    }
    */

}
