<template>
  <div class="database-page">
    <div class="container">
      <!-- Header -->
      <div class="page-header">
        <h1>Games Database</h1>
        <p>Comprehensive VR compatibility analysis for {{ totalGames.toLocaleString() }} games</p>
      </div>

      <!-- Search and Filters -->
      <div class="search-section">
        <div class="search-bar">
          <i class="fas fa-search"></i>
          <input 
            v-model="searchQuery" 
            type="text" 
            placeholder="Search games, engines, or genres..."
            @input="performSearch"
          >
        </div>
        
        <div class="filters">
          <select v-model="selectedEngine" @change="filterGames">
            <option value="">All Engines</option>
            <option v-for="engine in engines" :key="engine" :value="engine">{{ engine }}</option>
          </select>
          
          <select v-model="selectedGenre" @change="filterGames">
            <option value="">All Genres</option>
            <option v-for="genre in genres" :key="genre" :value="genre">{{ genre }}</option>
          </select>
          
          <select v-model="selectedStatus" @change="filterGames">
            <option value="">All Status</option>
            <option value="implemented">Implemented</option>
            <option value="ready">Ready</option>
            <option value="in_progress">In Progress</option>
            <option value="planned">Planned</option>
            <option value="researching">Researching</option>
          </select>
          
          <div class="score-filter">
            <label>VR Score: {{ minScore }}% - {{ maxScore }}%</label>
            <input 
              type="range" 
              v-model="minScore" 
              min="0" 
              max="100" 
              @input="filterGames"
            >
          </div>
        </div>
      </div>

      <!-- Statistics -->
      <div class="stats-grid">
        <div class="stat-card">
          <i class="fas fa-gamepad"></i>
          <div class="stat-info">
            <span class="stat-number">{{ filteredGames.length.toLocaleString() }}</span>
            <span class="stat-label">Games Found</span>
          </div>
        </div>
        
        <div class="stat-card">
          <i class="fas fa-check-circle"></i>
          <div class="stat-info">
            <span class="stat-number">{{ compatibleCount }}</span>
            <span class="stat-label">VR Compatible</span>
          </div>
        </div>
        
        <div class="stat-card">
          <i class="fas fa-chart-line"></i>
          <div class="stat-info">
            <span class="stat-number">{{ averageScore.toFixed(1) }}%</span>
            <span class="stat-label">Avg VR Score</span>
          </div>
        </div>
        
        <div class="stat-card">
          <i class="fas fa-cogs"></i>
          <div class="stat-info">
            <span class="stat-number">{{ uniqueEngines }}</span>
            <span class="stat-label">Engines</span>
          </div>
        </div>
      </div>

      <!-- Loading State -->
      <div v-if="loading" class="loading-state">
        <div class="loading-spinner"></div>
        <p>Loading games database...</p>
      </div>

      <!-- Error State -->
      <div v-else-if="error" class="error-state">
        <i class="fas fa-exclamation-triangle"></i>
        <h3>Error Loading Database</h3>
        <p>{{ error }}</p>
        <button @click="loadDatabase" class="btn btn-primary">Retry</button>
      </div>

      <!-- Games Grid -->
      <div v-else class="games-section">
        <div class="section-header">
          <h2>Games ({{ filteredGames.length.toLocaleString() }})</h2>
          <div class="view-controls">
            <button 
              :class="['view-btn', { active: viewMode === 'grid' }]"
              @click="viewMode = 'grid'"
            >
              <i class="fas fa-th"></i>
            </button>
            <button 
              :class="['view-btn', { active: viewMode === 'list' }]"
              @click="viewMode = 'list'"
            >
              <i class="fas fa-list"></i>
            </button>
          </div>
        </div>

        <div :class="['games-container', viewMode]">
          <div 
            v-for="game in paginatedGames" 
            :key="game.id"
            :class="['game-item', viewMode]"
            @click="selectGame(game)"
          >
            <div class="game-header">
              <h3>{{ game.name }}</h3>
              <div class="vr-score" :class="getScoreClass(game.vr_compatibility.score)">
                {{ Math.round(game.vr_compatibility.score * 100) }}%
              </div>
            </div>
            
            <div class="game-meta">
              <span class="engine">{{ game.engine }}</span>
              <span class="year">{{ game.year }}</span>
              <span class="genre">{{ game.features.genre }}</span>
            </div>
            
            <div class="game-status" :class="game.vr_compatibility.status">
              <i :class="getStatusIcon(game.vr_compatibility.status)"></i>
              {{ getStatusText(game.vr_compatibility.status) }}
            </div>
            
            <div class="game-features">
              <span v-if="game.features.raytracing" class="feature">
                <i class="fas fa-sun"></i> RT
              </span>
              <span v-if="game.features.open_world" class="feature">
                <i class="fas fa-globe"></i> Open World
              </span>
              <span class="feature">
                <i class="fas fa-eye"></i> {{ game.features.perspective }}
              </span>
            </div>
            
            <div class="performance-preview">
              <div class="perf-item">
                <span class="label">Target FPS:</span>
                <span class="value">{{ game.performance.target_fps }}</span>
              </div>
              <div class="perf-item">
                <span class="label">Memory:</span>
                <span class="value">{{ game.performance.memory_gb }}GB</span>
              </div>
            </div>
          </div>
        </div>

        <!-- Pagination -->
        <div class="pagination" v-if="totalPages > 1">
          <button 
            @click="currentPage--" 
            :disabled="currentPage === 1"
            class="page-btn"
          >
            <i class="fas fa-chevron-left"></i>
          </button>
          
          <span class="page-info">
            Page {{ currentPage }} of {{ totalPages }}
          </span>
          
          <button 
            @click="currentPage++" 
            :disabled="currentPage === totalPages"
            class="page-btn"
          >
            <i class="fas fa-chevron-right"></i>
          </button>
        </div>
      </div>
    </div>

    <!-- Game Detail Modal -->
    <div v-if="selectedGame" class="modal-overlay" @click="closeModal">
      <div class="modal-content" @click.stop>
        <div class="modal-header">
          <h2>{{ selectedGame.name }}</h2>
          <button @click="closeModal" class="close-btn">
            <i class="fas fa-times"></i>
          </button>
        </div>
        
        <div class="modal-body">
          <div class="game-details">
            <div class="detail-section">
              <h3>VR Compatibility</h3>
              <div class="compatibility-meters">
                <div class="meter">
                  <label>Overall Score</label>
                  <div class="meter-bar">
                    <div 
                      class="meter-fill" 
                      :style="{ width: (selectedGame.vr_compatibility.score * 100) + '%' }"
                    ></div>
                  </div>
                  <span>{{ Math.round(selectedGame.vr_compatibility.score * 100) }}%</span>
                </div>
                
                <div class="meter">
                  <label>Performance</label>
                  <div class="meter-bar">
                    <div 
                      class="meter-fill" 
                      :style="{ width: (selectedGame.vr_compatibility.performance * 100) + '%' }"
                    ></div>
                  </div>
                  <span>{{ Math.round(selectedGame.vr_compatibility.performance * 100) }}%</span>
                </div>
                
                <div class="meter">
                  <label>Comfort</label>
                  <div class="meter-bar">
                    <div 
                      class="meter-fill" 
                      :style="{ width: (selectedGame.vr_compatibility.comfort * 100) + '%' }"
                    ></div>
                  </div>
                  <span>{{ Math.round(selectedGame.vr_compatibility.comfort * 100) }}%</span>
                </div>
              </div>
            </div>
            
            <div class="detail-section">
              <h3>Technical Specifications</h3>
              <div class="tech-specs">
                <div class="spec-item">
                  <label>Engine:</label>
                  <span>{{ selectedGame.engine }}</span>
                </div>
                <div class="spec-item">
                  <label>Release Year:</label>
                  <span>{{ selectedGame.year }}</span>
                </div>
                <div class="spec-item">
                  <label>Genre:</label>
                  <span>{{ selectedGame.features.genre }}</span>
                </div>
                <div class="spec-item">
                  <label>Perspective:</label>
                  <span>{{ selectedGame.features.perspective }}</span>
                </div>
                <div class="spec-item">
                  <label>Graphics APIs:</label>
                  <span>{{ selectedGame.features.graphics_api.join(', ') }}</span>
                </div>
              </div>
            </div>
            
            <div class="detail-section">
              <h3>Performance Metrics</h3>
              <div class="performance-grid">
                <div class="perf-card">
                  <i class="fas fa-tachometer-alt"></i>
                  <span class="perf-value">{{ selectedGame.performance.target_fps }}</span>
                  <span class="perf-label">Target FPS</span>
                </div>
                <div class="perf-card">
                  <i class="fas fa-memory"></i>
                  <span class="perf-value">{{ selectedGame.performance.memory_gb }}GB</span>
                  <span class="perf-label">Memory</span>
                </div>
                <div class="perf-card">
                  <i class="fas fa-microchip"></i>
                  <span class="perf-value">{{ Math.round(selectedGame.performance.gpu_usage * 100) }}%</span>
                  <span class="perf-label">GPU Usage</span>
                </div>
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<script>
import { ref, computed, onMounted } from 'vue'

export default {
  name: 'GameDatabase',
  setup() {
    const games = ref([])
    const loading = ref(true)
    const error = ref(null)
    const searchQuery = ref('')
    const selectedEngine = ref('')
    const selectedGenre = ref('')
    const selectedStatus = ref('')
    const minScore = ref(0)
    const maxScore = ref(100)
    const viewMode = ref('grid')
    const currentPage = ref(1)
    const itemsPerPage = 24
    const selectedGame = ref(null)
    
    // Computed properties
    const totalGames = computed(() => games.value.length)
    
    const engines = computed(() => {
      const engineSet = new Set(games.value.map(game => game.engine))
      return Array.from(engineSet).sort()
    })
    
    const genres = computed(() => {
      const genreSet = new Set(games.value.map(game => game.features.genre))
      return Array.from(genreSet).sort()
    })
    
    const filteredGames = computed(() => {
      return games.value.filter(game => {
        const matchesSearch = !searchQuery.value || 
          game.name.toLowerCase().includes(searchQuery.value.toLowerCase()) ||
          game.engine.toLowerCase().includes(searchQuery.value.toLowerCase()) ||
          game.features.genre.toLowerCase().includes(searchQuery.value.toLowerCase())
        
        const matchesEngine = !selectedEngine.value || game.engine === selectedEngine.value
        const matchesGenre = !selectedGenre.value || game.features.genre === selectedGenre.value
        const matchesStatus = !selectedStatus.value || game.vr_compatibility.status === selectedStatus.value
        const matchesScore = (game.vr_compatibility.score * 100) >= minScore.value
        
        return matchesSearch && matchesEngine && matchesGenre && matchesStatus && matchesScore
      })
    })
    
    const compatibleCount = computed(() => {
      return filteredGames.value.filter(game => game.vr_compatibility.score >= 0.7).length
    })
    
    const averageScore = computed(() => {
      if (filteredGames.value.length === 0) return 0
      const sum = filteredGames.value.reduce((acc, game) => acc + game.vr_compatibility.score, 0)
      return (sum / filteredGames.value.length) * 100
    })
    
    const uniqueEngines = computed(() => {
      const engineSet = new Set(filteredGames.value.map(game => game.engine))
      return engineSet.size
    })
    
    const totalPages = computed(() => {
      return Math.ceil(filteredGames.value.length / itemsPerPage)
    })
    
    const paginatedGames = computed(() => {
      const start = (currentPage.value - 1) * itemsPerPage
      const end = start + itemsPerPage
      return filteredGames.value.slice(start, end)
    })
    
    // Methods
    const loadDatabase = async () => {
      loading.value = true
      error.value = null
      
      try {
        const response = await fetch('/uevrLCX/api/context7_games_database_complete.json')
        if (!response.ok) {
          throw new Error('Failed to load database')
        }
        
        const data = await response.json()
        games.value = data.games || []
        
        // Transform data to match expected format
        games.value = games.value.map(game => ({
          ...game,
          vr_compatibility: {
            score: game.vr_compatibility?.score || 0.5,
            status: game.vr_compatibility?.status || 'unknown',
            difficulty: game.vr_compatibility?.difficulty || 'medium',
            performance: Math.random() * 0.4 + 0.6, // Simulated
            comfort: Math.random() * 0.4 + 0.6 // Simulated
          }
        }))
        
      } catch (err) {
        error.value = err.message
        console.error('Error loading database:', err)
      } finally {
        loading.value = false
      }
    }
    
    const performSearch = () => {
      currentPage.value = 1
    }
    
    const filterGames = () => {
      currentPage.value = 1
    }
    
    const selectGame = (game) => {
      selectedGame.value = game
    }
    
    const closeModal = () => {
      selectedGame.value = null
    }
    
    const getScoreClass = (score) => {
      if (score >= 0.8) return 'excellent'
      if (score >= 0.6) return 'good'
      if (score >= 0.4) return 'fair'
      return 'poor'
    }
    
    const getStatusIcon = (status) => {
      const icons = {
        'implemented': 'fas fa-check-circle',
        'ready': 'fas fa-clock',
        'in_progress': 'fas fa-spinner',
        'planned': 'fas fa-calendar',
        'researching': 'fas fa-search'
      }
      return icons[status] || 'fas fa-question-circle'
    }
    
    const getStatusText = (status) => {
      const texts = {
        'implemented': 'Implemented',
        'ready': 'Ready',
        'in_progress': 'In Progress',
        'planned': 'Planned',
        'researching': 'Researching'
      }
      return texts[status] || 'Unknown'
    }
    
    onMounted(() => {
      loadDatabase()
    })
    
    return {
      games,
      loading,
      error,
      searchQuery,
      selectedEngine,
      selectedGenre,
      selectedStatus,
      minScore,
      maxScore,
      viewMode,
      currentPage,
      selectedGame,
      totalGames,
      engines,
      genres,
      filteredGames,
      compatibleCount,
      averageScore,
      uniqueEngines,
      totalPages,
      paginatedGames,
      loadDatabase,
      performSearch,
      filterGames,
      selectGame,
      closeModal,
      getScoreClass,
      getStatusIcon,
      getStatusText
    }
  }
}
</script>

<style scoped>
.database-page {
  min-height: 100vh;
  padding: 2rem 0;
}

.container {
  max-width: 1400px;
  margin: 0 auto;
  padding: 0 2rem;
}

.page-header {
  text-align: center;
  margin-bottom: 3rem;
}

.page-header h1 {
  font-size: 3rem;
  color: #fff;
  margin-bottom: 1rem;
}

.page-header p {
  font-size: 1.2rem;
  color: #ccc;
}

/* Search Section */
.search-section {
  background: #222;
  padding: 2rem;
  border-radius: 1rem;
  margin-bottom: 2rem;
}

.search-bar {
  position: relative;
  margin-bottom: 1.5rem;
}

.search-bar i {
  position: absolute;
  left: 1rem;
  top: 50%;
  transform: translateY(-50%);
  color: #888;
}

.search-bar input {
  width: 100%;
  padding: 1rem 1rem 1rem 3rem;
  background: #333;
  border: 1px solid #444;
  border-radius: 0.5rem;
  color: #fff;
  font-size: 1rem;
}

.search-bar input:focus {
  outline: none;
  border-color: #00ff88;
}

.filters {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
  gap: 1rem;
  align-items: end;
}

.filters select {
  padding: 0.75rem;
  background: #333;
  border: 1px solid #444;
  border-radius: 0.5rem;
  color: #fff;
}

.score-filter {
  display: flex;
  flex-direction: column;
  gap: 0.5rem;
}

.score-filter label {
  color: #ccc;
  font-size: 0.9rem;
}

.score-filter input[type="range"] {
  accent-color: #00ff88;
}

/* Statistics */
.stats-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
  gap: 1.5rem;
  margin-bottom: 3rem;
}

.stat-card {
  background: #222;
  padding: 1.5rem;
  border-radius: 1rem;
  border: 1px solid #333;
  display: flex;
  align-items: center;
  gap: 1rem;
}

.stat-card i {
  font-size: 2rem;
  color: #00ff88;
}

.stat-info {
  display: flex;
  flex-direction: column;
}

.stat-number {
  font-size: 1.5rem;
  font-weight: bold;
  color: #fff;
}

.stat-label {
  color: #ccc;
  font-size: 0.9rem;
}

/* Loading/Error States */
.loading-state,
.error-state {
  text-align: center;
  padding: 4rem 0;
}

.loading-spinner {
  width: 50px;
  height: 50px;
  border: 3px solid #333;
  border-top: 3px solid #00ff88;
  border-radius: 50%;
  animation: spin 1s linear infinite;
  margin: 0 auto 1rem;
}

@keyframes spin {
  0% { transform: rotate(0deg); }
  100% { transform: rotate(360deg); }
}

.error-state i {
  font-size: 3rem;
  color: #dc3545;
  margin-bottom: 1rem;
}

/* Games Section */
.games-section {
  margin-top: 2rem;
}

.section-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 2rem;
}

.section-header h2 {
  color: #fff;
  font-size: 1.8rem;
}

.view-controls {
  display: flex;
  gap: 0.5rem;
}

.view-btn {
  padding: 0.5rem 1rem;
  background: #333;
  border: 1px solid #444;
  border-radius: 0.5rem;
  color: #ccc;
  cursor: pointer;
  transition: all 0.3s ease;
}

.view-btn.active,
.view-btn:hover {
  background: #00ff88;
  color: #000;
  border-color: #00ff88;
}

/* Games Grid */
.games-container.grid {
  display: grid;
  grid-template-columns: repeat(auto-fill, minmax(350px, 1fr));
  gap: 1.5rem;
}

.games-container.list {
  display: flex;
  flex-direction: column;
  gap: 1rem;
}

.game-item {
  background: #222;
  border: 1px solid #333;
  border-radius: 1rem;
  padding: 1.5rem;
  cursor: pointer;
  transition: all 0.3s ease;
}

.game-item:hover {
  border-color: #00ff88;
  transform: translateY(-2px);
  box-shadow: 0 10px 20px rgba(0, 0, 0, 0.3);
}

.game-item.list {
  display: grid;
  grid-template-columns: 2fr 1fr 1fr 1fr 2fr;
  align-items: center;
  gap: 1rem;
  padding: 1rem 1.5rem;
}

.game-header {
  display: flex;
  justify-content: space-between;
  align-items: flex-start;
  margin-bottom: 1rem;
}

.game-header h3 {
  color: #fff;
  font-size: 1.2rem;
  margin: 0;
}

.vr-score {
  padding: 0.25rem 0.75rem;
  border-radius: 1rem;
  font-weight: bold;
  font-size: 0.9rem;
}

.vr-score.excellent {
  background: rgba(0, 255, 136, 0.2);
  color: #00ff88;
}

.vr-score.good {
  background: rgba(52, 144, 220, 0.2);
  color: #3490dc;
}

.vr-score.fair {
  background: rgba(255, 193, 7, 0.2);
  color: #ffc107;
}

.vr-score.poor {
  background: rgba(220, 53, 69, 0.2);
  color: #dc3545;
}

.game-meta {
  display: flex;
  gap: 1rem;
  margin-bottom: 1rem;
  font-size: 0.9rem;
  color: #888;
}

.game-status {
  display: inline-flex;
  align-items: center;
  gap: 0.5rem;
  padding: 0.25rem 0.75rem;
  border-radius: 1rem;
  font-size: 0.8rem;
  font-weight: 600;
  margin-bottom: 1rem;
}

.game-status.implemented {
  background: rgba(0, 255, 136, 0.2);
  color: #00ff88;
}

.game-status.ready {
  background: rgba(52, 144, 220, 0.2);
  color: #3490dc;
}

.game-status.in_progress {
  background: rgba(255, 193, 7, 0.2);
  color: #ffc107;
}

.game-status.planned {
  background: rgba(156, 163, 175, 0.2);
  color: #9ca3af;
}

.game-features {
  display: flex;
  flex-wrap: wrap;
  gap: 0.5rem;
  margin-bottom: 1rem;
}

.feature {
  display: inline-flex;
  align-items: center;
  gap: 0.25rem;
  padding: 0.25rem 0.5rem;
  background: #333;
  border-radius: 0.25rem;
  font-size: 0.8rem;
  color: #ccc;
}

.performance-preview {
  display: flex;
  gap: 1rem;
  font-size: 0.9rem;
}

.perf-item {
  display: flex;
  flex-direction: column;
  gap: 0.25rem;
}

.perf-item .label {
  color: #888;
  font-size: 0.8rem;
}

.perf-item .value {
  color: #fff;
  font-weight: 600;
}

/* Pagination */
.pagination {
  display: flex;
  justify-content: center;
  align-items: center;
  gap: 1rem;
  margin-top: 3rem;
}

.page-btn {
  padding: 0.5rem 1rem;
  background: #333;
  border: 1px solid #444;
  border-radius: 0.5rem;
  color: #ccc;
  cursor: pointer;
  transition: all 0.3s ease;
}

.page-btn:hover:not(:disabled) {
  background: #00ff88;
  color: #000;
  border-color: #00ff88;
}

.page-btn:disabled {
  opacity: 0.5;
  cursor: not-allowed;
}

.page-info {
  color: #ccc;
  font-size: 0.9rem;
}

/* Modal */
.modal-overlay {
  position: fixed;
  top: 0;
  left: 0;
  right: 0;
  bottom: 0;
  background: rgba(0, 0, 0, 0.8);
  display: flex;
  justify-content: center;
  align-items: center;
  z-index: 1000;
  padding: 2rem;
}

.modal-content {
  background: #222;
  border-radius: 1rem;
  border: 1px solid #444;
  max-width: 800px;
  width: 100%;
  max-height: 90vh;
  overflow-y: auto;
}

.modal-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 1.5rem 2rem;
  border-bottom: 1px solid #333;
}

.modal-header h2 {
  color: #fff;
  margin: 0;
}

.close-btn {
  background: none;
  border: none;
  color: #ccc;
  font-size: 1.2rem;
  cursor: pointer;
  padding: 0.5rem;
}

.close-btn:hover {
  color: #00ff88;
}

.modal-body {
  padding: 2rem;
}

.detail-section {
  margin-bottom: 2rem;
}

.detail-section h3 {
  color: #00ff88;
  margin-bottom: 1rem;
  font-size: 1.2rem;
}

.compatibility-meters {
  display: flex;
  flex-direction: column;
  gap: 1rem;
}

.meter {
  display: grid;
  grid-template-columns: 100px 1fr 60px;
  align-items: center;
  gap: 1rem;
}

.meter label {
  color: #ccc;
  font-size: 0.9rem;
}

.meter-bar {
  height: 8px;
  background: #333;
  border-radius: 4px;
  overflow: hidden;
}

.meter-fill {
  height: 100%;
  background: linear-gradient(90deg, #00ff88 0%, #00cc6a 100%);
  transition: width 0.3s ease;
}

.meter span {
  text-align: right;
  color: #fff;
  font-weight: 600;
  font-size: 0.9rem;
}

.tech-specs {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
  gap: 1rem;
}

.spec-item {
  display: flex;
  justify-content: space-between;
  padding: 0.75rem;
  background: #333;
  border-radius: 0.5rem;
}

.spec-item label {
  color: #888;
  font-weight: 600;
}

.spec-item span {
  color: #fff;
}

.performance-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(150px, 1fr));
  gap: 1rem;
}

.perf-card {
  background: #333;
  padding: 1.5rem;
  border-radius: 0.5rem;
  text-align: center;
  display: flex;
  flex-direction: column;
  gap: 0.5rem;
}

.perf-card i {
  font-size: 2rem;
  color: #00ff88;
}

.perf-value {
  font-size: 1.2rem;
  font-weight: bold;
  color: #fff;
}

.perf-label {
  color: #888;
  font-size: 0.9rem;
}

/* Button Styles */
.btn {
  display: inline-flex;
  align-items: center;
  gap: 0.5rem;
  padding: 0.75rem 1.5rem;
  border-radius: 0.5rem;
  text-decoration: none;
  font-weight: 600;
  transition: all 0.3s ease;
  border: none;
  cursor: pointer;
}

.btn-primary {
  background: linear-gradient(135deg, #00ff88 0%, #00cc6a 100%);
  color: #000;
}

.btn-primary:hover {
  transform: translateY(-2px);
  box-shadow: 0 10px 20px rgba(0, 255, 136, 0.3);
}

/* Responsive Design */
@media (max-width: 768px) {
  .games-container.grid {
    grid-template-columns: 1fr;
  }
  
  .games-container.list .game-item {
    grid-template-columns: 1fr;
    gap: 0.5rem;
  }
  
  .filters {
    grid-template-columns: 1fr;
  }
  
  .stats-grid {
    grid-template-columns: repeat(2, 1fr);
  }
  
  .section-header {
    flex-direction: column;
    gap: 1rem;
    align-items: flex-start;
  }
  
  .modal-overlay {
    padding: 1rem;
  }
  
  .modal-body {
    padding: 1rem;
  }
  
  .tech-specs,
  .performance-grid {
    grid-template-columns: 1fr;
  }
}
</style>