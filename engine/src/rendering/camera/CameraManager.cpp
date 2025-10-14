#include "../../../include/rendering/camera/CameraManager.h"

int CameraManager::addCamera(std::unique_ptr<Camera> camera, const std::string& name) {
    if (!camera) {
        std::cerr << "Warning: Attempted to add null camera to CameraManager" << std::endl;
        return -1;
    }
    
    m_cameras.push_back(std::move(camera));
    m_cameraNames.push_back(name);
    
    int index = static_cast<int>(m_cameras.size() - 1);
    
    // If this is the first camera, make it active
    if (m_cameras.size() == 1) {
        m_activeCameraIndex = 0;
        std::cout << "Camera System: Added first camera '" << name << "' (active)" << std::endl;
    } else {
        std::cout << "Camera System: Added camera '" << name << "' at index " << index << std::endl;
    }
    
    return index;
}

void CameraManager::removeCamera(int index) {
    if (index < 0 || index >= static_cast<int>(m_cameras.size())) {
        std::cerr << "Warning: Invalid camera index " << index << " for removal" << std::endl;
        return;
    }
    
    std::string removedName = m_cameraNames[index];
    m_cameras.erase(m_cameras.begin() + index);
    m_cameraNames.erase(m_cameraNames.begin() + index);
    
    // Adjust active camera index if necessary
    if (m_activeCameraIndex >= index && m_activeCameraIndex > 0) {
        m_activeCameraIndex--;
    }
    
    // If we removed all cameras, reset active index
    if (m_cameras.empty()) {
        m_activeCameraIndex = 0;
    }
    
    std::cout << "Camera System: Removed camera '" << removedName << "'" << std::endl;
}

void CameraManager::switchToCamera(int index) {
    if (index < 0 || index >= static_cast<int>(m_cameras.size())) {
        std::cerr << "Warning: Invalid camera index " << index << " for switching" << std::endl;
        return;
    }
    
    if (index == m_activeCameraIndex) {
        return; // Already active
    }
    
    int previousIndex = m_activeCameraIndex;
    m_activeCameraIndex = index;
    
    std::cout << "Camera System: Switched from '" << m_cameraNames[previousIndex] 
              << "' to '" << m_cameraNames[index] << "'" << std::endl;
}

void CameraManager::switchToNextCamera() {
    if (m_cameras.empty()) {
        return;
    }
    
    int nextIndex = (m_activeCameraIndex + 1) % static_cast<int>(m_cameras.size());
    switchToCamera(nextIndex);
}

void CameraManager::switchToPreviousCamera() {
    if (m_cameras.empty()) {
        return;
    }
    
    int prevIndex = (m_activeCameraIndex - 1 + static_cast<int>(m_cameras.size())) % static_cast<int>(m_cameras.size());
    switchToCamera(prevIndex);
}

Camera* CameraManager::getActiveCamera() {
    if (m_cameras.empty() || m_activeCameraIndex < 0 || m_activeCameraIndex >= static_cast<int>(m_cameras.size())) {
        return nullptr;
    }
    
    return m_cameras[m_activeCameraIndex].get();
}

const std::string& CameraManager::getActiveCameraName() const {
    static const std::string empty = "";
    
    if (m_cameraNames.empty() || m_activeCameraIndex < 0 || m_activeCameraIndex >= static_cast<int>(m_cameraNames.size())) {
        return empty;
    }
    
    return m_cameraNames[m_activeCameraIndex];
}

void CameraManager::updateAllCameras(GLFWwindow* window, float deltaTime) {
    for (auto& camera : m_cameras) {
        if (camera) {
            camera->update(window, deltaTime);
        }
    }
}

Camera* CameraManager::getCamera(size_t index) const {
    if (index >= m_cameras.size()) {
        return nullptr;
    }
    return m_cameras[index].get();
}
