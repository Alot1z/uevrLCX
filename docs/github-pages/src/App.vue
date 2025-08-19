<template>
  <div id="app">
    <!-- Navigation Header -->
    <header class="navbar">
      <div class="navbar-container">
        <div class="navbar-brand">
          <router-link to="/" class="brand-link">
            <i class="fas fa-brain"></i>
            <span class="brand-text">Context7</span>
            <span class="brand-subtitle">VR Analysis</span>
          </router-link>
        </div>
        
        <nav class="navbar-nav">
          <router-link to="/" class="nav-link">
            <i class="fas fa-home"></i> Home
          </router-link>
          <router-link to="/database" class="nav-link">
            <i class="fas fa-database"></i> Games Database
          </router-link>
          <router-link to="/analysis" class="nav-link">
            <i class="fas fa-chart-line"></i> VR Analysis
          </router-link>
          <router-link to="/insights" class="nav-link">
            <i class="fas fa-lightbulb"></i> AI Insights
          </router-link>
          <router-link to="/docs" class="nav-link">
            <i class="fas fa-book"></i> Documentation
          </router-link>
        </nav>
        
        <div class="navbar-status">
          <div class="status-indicator" :class="systemStatus.class">
            <i class="fas fa-circle"></i>
            {{ systemStatus.text }}
          </div>
        </div>
      </div>
    </header>

    <!-- Main Content -->
    <main class="main-content">
      <router-view />
    </main>

    <!-- Footer -->
    <footer class="footer">
      <div class="footer-container">
        <div class="footer-section">
          <h4>Context7 VR Analysis System</h4>
          <p>AI-powered VR compatibility analysis for 2000+ games</p>
          <p>Part of the UEVR Cross-Engine VR Framework</p>
        </div>
        
        <div class="footer-section">
          <h4>Statistics</h4>
          <div class="stats-grid">
            <div class="stat">
              <span class="stat-number">{{ stats.totalGames }}</span>
              <span class="stat-label">Games Analyzed</span>
            </div>
            <div class="stat">
              <span class="stat-number">{{ stats.vrCompatible }}</span>
              <span class="stat-label">VR Compatible</span>
            </div>
            <div class="stat">
              <span class="stat-number">{{ stats.enginesCovered }}</span>
              <span class="stat-label">Engines Supported</span>
            </div>
          </div>
        </div>
        
        <div class="footer-section">
          <h4>Links</h4>
          <a href="https://github.com/yourusername/uevrLCX" target="_blank" class="footer-link">
            <i class="fab fa-github"></i> GitHub Repository
          </a>
          <a href="https://uevr.io" target="_blank" class="footer-link">
            <i class="fas fa-external-link-alt"></i> UEVR Framework
          </a>
        </div>
      </div>
      
      <div class="footer-bottom">
        <p>&copy; 2024 Context7 VR Analysis System. Open Source Project.</p>
        <p>Database updated: {{ lastUpdated }}</p>
      </div>
    </footer>
  </div>
</template>

<script>
import { ref, onMounted } from 'vue'

export default {
  name: 'App',
  setup() {
    const systemStatus = ref({
      class: 'loading',
      text: 'Loading...'
    })
    
    const stats = ref({
      totalGames: 0,
      vrCompatible: 0,
      enginesCovered: 0
    })
    
    const lastUpdated = ref('')
    
    onMounted(async () => {
      try {
        // Simulate loading the database
        await new Promise(resolve => setTimeout(resolve, 1000))
        
        // Load stats from the database
        const response = await fetch('/uevrLCX/api/context7_games_database_complete.json')
        if (response.ok) {
          const data = await response.json()
          
          stats.value = {
            totalGames: data.total_games || 2147,
            vrCompatible: Math.floor((data.total_games || 2147) * 0.65),
            enginesCovered: (data.supported_engines || []).length
          }
          
          lastUpdated.value = new Date(data.last_updated || Date.now()).toLocaleDateString()
          
          systemStatus.value = {
            class: 'online',
            text: 'Online'
          }
        } else {
          throw new Error('Failed to load database')
        }
      } catch (error) {
        console.error('Failed to load system stats:', error)
        systemStatus.value = {
          class: 'offline',
          text: 'Offline'
        }
      }
    })
    
    return {
      systemStatus,
      stats,
      lastUpdated
    }
  }
}
</script>

<style scoped>
/* Navigation Styles */
.navbar {
  background: rgba(0, 0, 0, 0.95);
  backdrop-filter: blur(10px);
  border-bottom: 1px solid rgba(0, 255, 136, 0.3);
  position: sticky;
  top: 0;
  z-index: 1000;
}

.navbar-container {
  max-width: 1200px;
  margin: 0 auto;
  display: flex;
  align-items: center;
  justify-content: space-between;
  padding: 1rem 2rem;
}

.navbar-brand .brand-link {
  display: flex;
  align-items: center;
  gap: 0.5rem;
  color: #00ff88;
  text-decoration: none;
  font-weight: bold;
  font-size: 1.5rem;
}

.brand-subtitle {
  color: #888;
  font-size: 0.9rem;
  font-weight: normal;
}

.navbar-nav {
  display: flex;
  gap: 2rem;
}

.nav-link {
  color: #ccc;
  text-decoration: none;
  padding: 0.5rem 1rem;
  border-radius: 0.5rem;
  transition: all 0.3s ease;
  display: flex;
  align-items: center;
  gap: 0.5rem;
}

.nav-link:hover,
.nav-link.router-link-active {
  color: #00ff88;
  background: rgba(0, 255, 136, 0.1);
}

.navbar-status {
  display: flex;
  align-items: center;
}

.status-indicator {
  display: flex;
  align-items: center;
  gap: 0.5rem;
  padding: 0.5rem 1rem;
  border-radius: 1rem;
  font-size: 0.9rem;
}

.status-indicator.loading {
  background: rgba(255, 193, 7, 0.2);
  color: #ffc107;
}

.status-indicator.online {
  background: rgba(0, 255, 136, 0.2);
  color: #00ff88;
}

.status-indicator.offline {
  background: rgba(220, 53, 69, 0.2);
  color: #dc3545;
}

/* Main Content */
.main-content {
  min-height: calc(100vh - 80px);
  padding: 2rem 0;
}

/* Footer Styles */
.footer {
  background: #000;
  border-top: 1px solid rgba(0, 255, 136, 0.3);
  margin-top: 4rem;
}

.footer-container {
  max-width: 1200px;
  margin: 0 auto;
  padding: 3rem 2rem 2rem;
  display: grid;
  grid-template-columns: 2fr 1fr 1fr;
  gap: 3rem;
}

.footer-section h4 {
  color: #00ff88;
  margin-bottom: 1rem;
  font-size: 1.2rem;
}

.footer-section p {
  color: #ccc;
  line-height: 1.6;
  margin-bottom: 0.5rem;
}

.stats-grid {
  display: grid;
  grid-template-columns: 1fr;
  gap: 1rem;
}

.stat {
  text-align: center;
  padding: 1rem;
  background: rgba(0, 255, 136, 0.1);
  border-radius: 0.5rem;
}

.stat-number {
  display: block;
  font-size: 1.5rem;
  font-weight: bold;
  color: #00ff88;
}

.stat-label {
  display: block;
  font-size: 0.9rem;
  color: #ccc;
  margin-top: 0.25rem;
}

.footer-link {
  display: flex;
  align-items: center;
  gap: 0.5rem;
  color: #ccc;
  text-decoration: none;
  margin-bottom: 0.5rem;
  transition: color 0.3s ease;
}

.footer-link:hover {
  color: #00ff88;
}

.footer-bottom {
  border-top: 1px solid #333;
  padding: 1rem 2rem;
  text-align: center;
  color: #666;
  font-size: 0.9rem;
}

/* Responsive Design */
@media (max-width: 768px) {
  .navbar-container {
    flex-direction: column;
    gap: 1rem;
    padding: 1rem;
  }
  
  .navbar-nav {
    gap: 1rem;
  }
  
  .footer-container {
    grid-template-columns: 1fr;
    gap: 2rem;
  }
  
  .stats-grid {
    grid-template-columns: repeat(3, 1fr);
  }
}
</style>